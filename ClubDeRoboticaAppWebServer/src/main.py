from handlers import *
import Credentials as credentials
import threading
import time
from Database import *
from Encryption import *
from flask import Flask, request

import Credentials as credentials

active_Nonces = []
Webserver = Flask(__name__)


@Webserver.route('/S1', methods=['POST'])
def BeginCommunicationS1():
    if request.method == 'POST':
        RawPackage = request.data.decode('utf-8')
        print(f'Received data on /S1: {RawPackage}')
        Package = enc.decrypt(RawPackage, credentials.key)
        JsonDataReceived = json.loads(Package)
        print(JsonDataReceived)
        SecondNonce = base64.b64encode(get_random_bytes(16)).decode('utf-8')
        if len(active_Nonces) < 10:
            active_Nonces.Webserverend(SecondNonce)
        else:
            active_Nonces.clear()
            active_Nonces.Webserverend(SecondNonce)
        PackagetoSend = {'EncryptedFirstNonce': enc.encrypt(JsonDataReceived['FirstNonce'], credentials.key),
                         'SecondNonce': SecondNonce}
        print(PackagetoSend)
        encPackagetoSend = enc.encrypt(json.dumps(PackagetoSend), credentials.key)
        return encPackagetoSend


@Webserver.route('/S2', methods=['POST'])
def receivedataS2():
    if request.method == 'POST':
        RawPackageS2 = request.data.decode('utf-8')
        print(f'Received data on /S2: {RawPackageS2}')
        Package = enc.decrypt(RawPackageS2, credentials.key)
        JsonDataReceived = json.loads(Package)
        print(JsonDataReceived)
        EncryptedSecondNonce = enc.decrypt(JsonDataReceived['EncryptedSecondNonce'], credentials.key)
        if EncryptedSecondNonce in active_Nonces:
            active_Nonces.remove(EncryptedSecondNonce)
            operation_code = JsonDataReceived['Operation']
            PackagetoSend = Step2Response(operation_code, JsonDataReceived)
            print(PackagetoSend)
            encPackagetoSend = enc.encrypt(json.dumps(PackagetoSend), credentials.key)
            return encPackagetoSend
        return 'Nonce is not identified.'


@Webserver.route('/Announcements', methods=['GET'])
def Announcements():
    session = Session()
    stmt = session.query(AnnouncedItem).order_by(AnnouncedItem.a_id.desc()).limit(50)
    announced_items = stmt.all()
    json_output = [{'a_id': item.a_id, 'title': item.title, 'message': item.message, 'date': item.date} for item in
                   announced_items]
    print(json_output)
    session.close()
    return json.dumps(json_output)


@Webserver.route('/Weather', methods=['GET'])
def Weather():
    session = Session()
    latest_weather = session.query(WeatherData).order_by(WeatherData.w_id.desc()).first()
    if latest_weather:
        json_output = {'WindSpd': latest_weather.windspeed, 'WindDir': latest_weather.winddirection,
                       'UVidx': latest_weather.uvindex, 'Temp': latest_weather.temperature,
                       'RainG': latest_weather.raingauge, 'RelHum': latest_weather.relhum}
    else:
        json_output = {'error': 'No weather data available'}
    print(json_output)
    session.close()
    return json.dumps(json_output)


@Webserver.route('/Time', methods=['GET'])
def Time():
    current_time = time.localtime()
    tm_data = {'tm_sec': current_time.tm_sec, 'tm_min': current_time.tm_min, 'tm_hour': current_time.tm_hour,
               'tm_mday': current_time.tm_mday, 'tm_mon': current_time.tm_mon, 'tm_year': current_time.tm_year,
               'tm_wday': current_time.tm_wday, 'tm_yday': current_time.tm_yday, 'tm_isdst': current_time.tm_isdst}
    json_data = json.dumps(tm_data, separators=(',', ':'))
    return json_data


def AuxThread():
    while True:
        time.sleep(5)


if __name__ == '__main__':
    task_thread = threading.Thread(target=AuxThread, daemon=True)
    task_thread.start()
    Webserver.run(debug=True, host='0.0.0.0')
