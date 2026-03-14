import requests
import json
import Encryption as enc
import Credentials as credentials
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
from Crypto.Random import get_random_bytes
import base64

def encrypt(Plaintext, key):
    iv = get_random_bytes(16)
    cipher = AES.new(key, AES.MODE_CBC, iv)
    ciphertext = cipher.encrypt(pad(Plaintext.encode('utf-8'), AES.block_size))
    ciphertext = iv + ciphertext
    ciphertext = base64.b64encode(ciphertext).decode('utf-8')
    return ciphertext

def decrypt(ciphertext_base64, key):
    try:
        decoded_ciphertext = base64.b64decode(ciphertext_base64)
        iv = decoded_ciphertext[:16]
        actual_ciphertext = decoded_ciphertext[16:]
        cipher = AES.new(key, AES.MODE_CBC, iv)
        decrypted_bytes = cipher.decrypt(actual_ciphertext)
        plaintext = unpad(decrypted_bytes, AES.block_size).decode('utf-8')
        return plaintext
    except ValueError as e:
        print('Decryption error:', str(e))
        return None
    else:
        pass


def PostRequestEncrypted(baseurl, PackagetoSendS2):
    FirstNonce = base64.b64encode(get_random_bytes(16)).decode('utf-8')
    PackagetoSendS1 = {'FirstNonce': FirstNonce}
    print(PackagetoSendS1)
    encPackagetoSend = enc.encrypt(json.dumps(PackagetoSendS1), credentials.key)
    S1url = baseurl + '/S1'
    response = requests.post(S1url, data=encPackagetoSend)
    print(response.text)
    DecryptedResponseS1 = enc.decrypt(response.text, credentials.key)
    print(f'Received data from /S1: {DecryptedResponseS1}')
    JsonDataReceived = json.loads(DecryptedResponseS1)
    if enc.decrypt(JsonDataReceived['EncryptedFirstNonce'], credentials.key) == FirstNonce:
        PackagetoSendS2['EncryptedSecondNonce'] = enc.encrypt(JsonDataReceived['SecondNonce'], credentials.key)
        print(PackagetoSendS2)
        encPackagetoSendS2 = enc.encrypt(json.dumps(PackagetoSendS2), credentials.key)
        S2url = baseurl + '/S2'
        response = requests.post(S2url, data=encPackagetoSendS2)
        print(response.text)
        DecryptedResponseS2 = enc.decrypt(response.text, credentials.key)
        print(f'Received data from /S2: {DecryptedResponseS2}')
        return DecryptedResponseS2
    return 'Nonce validation failed in Step 1.'