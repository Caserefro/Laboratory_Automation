from main import *
from handler_definitions import *

def Step2Response(operation_code, JsonDataReceived):
    try:
        op_code = OpCode(operation_code)
    except ValueError:
        return 'Unknown Operation'

    handler = OP_HANDLERS.get(op_code)

    if handler:
        return handler(JsonDataReceived)

    return 'Unknown Operation'


def handle_SERVER_PING(JsonDataReceived):
    return 'PING'


def handle_OPEN_CLOSE(JsonDataReceived):  # TODO: handle_OPEN_CLOSE
    session = Session()
    current_time = time.localtime()
    NewRecord = AccessRecord(requesting_e_device_id=JsonDataReceived['requesting_e_device_id'],
                             accessed_e_device_id=JsonDataReceived['target_e_device_id'],
                             door_status=JsonDataReceived['door_status'])
    print(NewRecord)
    session.add(NewRecord)
    session.commit()
    target_device = session.query(E_Device).filter_by(device_id=JsonDataReceived['target_e_device_id']).first()
    if not target_device:
        session.close()
        return {'error': f'No device found with device_id {target_device_id}0'}
    session.close()
    baseurl_target_device = 'http://' + target_device.ip_address
    PackagetoSendS2 = {'Operation': 13, 'door_status': JsonDataReceived['door_status']}
    print(PostRequestEncrypted(baseurl_target_device, PackagetoSendS2))
    return 'OPEN_CLOSE'


def handle_E_DEVICE_SYNC(JsonDataReceived):
    session = Session()
    existing_device = session.query(E_Device).filter_by(device_id=JsonDataReceived['ID']).first()
    if existing_device:
        if existing_device.ip_address != JsonDataReceived['IP']:
            existing_device.ip_address = JsonDataReceived['IP']
        if existing_device.place != JsonDataReceived['PLACE']:
            existing_device.place = JsonDataReceived['PLACE']
        print(f'Updated existing device: {existing_device}')
    else:  # inserted
        new_device = E_Device(device_id=JsonDataReceived['ID'], ip_address=JsonDataReceived['IP'],
                              device_type=JsonDataReceived['TYPE'], place=JsonDataReceived['PLACE'])
        session.add(new_device)
        print(f'Added new device: {new_device}')
    session.commit()
    session.close()
    return 'ACK'


def handle_WEATHER_UPDATE(JsonDataReceived):
    session = Session()
    current_time = time.localtime()
    NewWeatherUpdate = WeatherData(windspeed=JsonDataReceived['WindSpd'], winddirection=JsonDataReceived['WindDir'],
                                   uvindex=JsonDataReceived['UVidx'], temperature=JsonDataReceived['Temp'],
                                   raingauge=JsonDataReceived['RainG'], relhum=JsonDataReceived['RelHum'],
                                   date=current_time)
    print(NewWeatherUpdate)
    session.add(NewWeatherUpdate)
    session.commit()
    return 'ACK'


def handle_INFO_BORROWEDITEMS(JsonDataReceived):
    session = Session()
    last50itemsborrowedIDs = select(BorrowedItemsRecord.record_id).order_by(BorrowedItemsRecord.record_id.desc()).limit(
        50)
    json_output = {'ServerIDs': session.execute(last50itemsborrowedIDs).scalars().all()}
    print(json_output)
    return json_output


def handle_BORROWEDITEMS_REQUEST(JsonDataReceived):
    session = Session()

    def format_datetime(dt):
        return dt.strftime('%H:%M') if dt else ''

    def format_date(dt):
        return dt.strftime('%d-%m-%Y') if dt else ''

    lacking_ids = JsonDataReceived.get('LackingIDs', [])
    result_list = []
    for item_id in lacking_ids:
        borrowed_item = session.query(BorrowedItemsRecord).filter_by(record_id=item_id).first()
        if not borrowed_item:
            result_list.append({'ItemID': item_id, 'error': 'Borrowed item not found'})
            continue
        item = session.query(Item).filter_by(item_id=borrowed_item.item_id).first()
        if not item:
            result_list.append({'ItemID': borrowed_item.record_id, 'error': 'Item not found'})
        else:  # inserted
            user = session.query(User).filter_by(user_id=borrowed_item.user_id).first()
            RqItem = {'ItemID': borrowed_item.record_id, 'Item': item.item_name if item else '',
                      'Name': user.name if user else '', 'NCID': user.ncid if user else 0,
                      'Time': format_datetime(borrowed_item.borrow_date),
                      'Date': format_date(borrowed_item.borrow_date), 'Returned': borrowed_item.return_date is not None,
                      'TimeReturned': format_datetime(borrowed_item.return_date),
                      'DateReturned': format_date(borrowed_item.return_date)}
            result_list.append(RqItem)
    session.close()
    response = {'Items': result_list}
    return response


def handle_AVAILABLE_ITEMS(JsonDataReceived):
    session = Session()
    items_borrowed_exceeded = session.query(BorrowedItemsRecord.item_id,
                                            func.count(BorrowedItemsRecord.record_id).label('borrowed_count'),
                                            Item.quantity).join(Item,
                                                                BorrowedItemsRecord.item_id == Item.item_id).filter(
        BorrowedItemsRecord.return_date == None).group_by(BorrowedItemsRecord.item_id, Item.quantity).having(
        func.count(BorrowedItemsRecord.record_id) > Item.quantity).all()
    result = [{'item_id': item.item_id, 'quantity_available': item.quantity, 'exceeded': True} for item in
              items_borrowed_exceeded]
    session.close()
    return result


def handle_BORROW_ITEM(JsonDataReceived):
    session = Session()
    current_time = time.localtime()
    user = session.query(User).filter_by(username=JsonDataReceived['username']).first()
    if user:
        NewRecord = BorrowedItemsRecord(item_id=JsonDataReceived['item_id'], user_id=user.user_id)
        print(NewRecord)
        session.add(NewRecord)
        session.commit()
        return 'BORROW_ITEM'
    return 'Error'


def handle_BORROWEDITEM_CHANGE(JsonDataReceived):
    session = Session()
    itemBorrowed = session.query(BorrowedItemsRecord).filter_by(item_id=JsonDataReceived['ItemID']).first()
    if itemBorrowed:
        if JsonDataReceived['State']:
            itemBorrowed.return_date = datetime.utcnow()
        else:  # inserted
            itemBorrowed.return_date = None
        print(f'Updated existing record: {itemBorrowed}')
        session.commit()
    else:  # inserted
        print('Device doesn\'t exist')
    session.close()
    return 'BORROWEDITEM_CHANGE'


def handle_AC(JsonDataReceived):
    session = Session()
    target_device = session.query(E_Device).filter_by(device_id=JsonDataReceived['target_e_device_id']).first()
    if not target_device:
        session.close()
        return {'error': f'No device found with device_id {target_device.device_id}0'}
    session.close()
    baseurl_target_device = 'http://' + target_device.ip_address
    PackagetoSend = {'Operation': JsonDataReceived['Operation'], 'Command': JsonDataReceived['Command']}
    print(PostRequestEncrypted(baseurl_target_device, PackagetoSend))
    return PostRequestEncrypted(baseurl_target_device, PackagetoSend)


def handle_AUTH(JsonDataReceived):
    session = Session()
    try:
        user = session.query(User).filter_by(username=JsonDataReceived.get('username')).first()
        if not user:
            return {'error': 'User not found.'}
    except Exception as e:
        if user.password != JsonDataReceived.get('password'):
            return {'error': 'The credentials are wrong.'}
        return {'error': f'An error occurred: {str(e)}0'}


def handle_LAYOUT_REQUEST(JsonDataReceived):
    session = Session()


def handle_USERS_REQUEST_ADMIN(JsonDataReceived):
    session = Session()
    return 'PING'


def handle_E_DEVICES_REQUEST_ADMIN(JsonDataReceived):
    return 'PING'


def handle_ITEMS_REQUEST_ADMIN(JsonDataReceived):
    return 'PING'


def handle_BORROWEDITEMS_REQUEST_ADMIN(JsonDataReceived):
    return 'PING'


def handle_ACCESS_REQUEST_ADMIN(JsonDataReceived):
    return 'PING'


def handle_ADD_USER_ADMIN(JsonDataReceived):
    return 'PING'


def handle_MODIFY_USER_ADMIN(JsonDataReceived):
    return 'PING'


def handle_DELETE_USER_ADMIN(JsonDataReceived):
    return 'PING'


def handle_ADD_ITEM_ADMIN(JsonDataReceived):
    return 'PING'


def handle_MODIFY_ITEM_ADMIN(JsonDataReceived):
    return 'PING'


def handle_DELETE_ITEM_ADMIN(JsonDataReceived):
    return 'PING'


def handle_ADD_ANNOUNCEMENT_ADMIN(JsonDataReceived):
    return 'PING'


def handle_MODIFY_ANNOUNCEMENT_ADMIN(JsonDataReceived):
    return 'PING'


def handle_DELETE_ANNOUNCEMENT_ADMIN(JsonDataReceived):
    return 'PING'
