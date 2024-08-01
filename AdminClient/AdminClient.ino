#include "ADMINCLIENT.h"
/*
This file is done to add users and recover records via http. 
why not python? time, mostly. (summers almost over.)
could be done in python? yeah.
will be done in python? maybe idk
*/

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  // SettingupPreferences();
  Serial.println("introduzca el codigo de la operacion que desee. ");
  delay(300);
}
void loop() {
  static int stage = 0;
  static int DesiredOperation = 0;
  static struct UserData NewUser;  // Declare NewUser outside switch
  static String CurrentTime;
  static String Username;
  static String Password;
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    if (!DesiredOperation)
      DesiredOperation = input.toInt();
    switch (DesiredOperation) {
      case OP_SERVER_PING:
        OP_SERVER_PING_Wrapper();
        DesiredOperation = 0;
        break;
      case OP_ADMIN_INFO:
        OP_ADMIN_INFO_Wrapper();
        DesiredOperation = 0;
        break;
      case OP_ADMIN_SETTIME:
        switch (stage) {
          case 0:
            Serial.println("Enter the Json containing the tm time Structure:");
            stage++;
            break;
          case 1:
            CurrentTime = input;
            Serial.println(CurrentTime);
            OP_ADMIN_SETTIME_Wrapper(CurrentTime);
            stage = 0;  // Reset stage
            DesiredOperation = 0;
            break;
        }
        break;
      case OP_ADMIN_ALLSTUDENTS:
        OP_ADMIN_ALLSTUDENTS_Wrapper();
        DesiredOperation = 0;
        break;
      case OP_ADMIN_ADDSTUDENT:
        switch (stage) {
          case 0:
            Serial.println("Please enter the following details:");
            Serial.println("Enter Username (max 20 characters):");
            stage++;
            break;
          case 1:
            NewUser.Username = input;
            Serial.println("Enter Full Name (max 40 characters):");
            stage++;
            break;
          case 2:
            NewUser.Name = input;
            Serial.println("Enter Numero de control:");
            stage++;
            break;
          case 3:
            NewUser.NCID = input.toInt();
            Serial.println("Enter Password: (16 Chars Maximum.)");
            stage++;
            break;
          case 4:
            Password = input;
            if (Password.length() <= 16) {
              // Calculate the number of bytes to pad
              int bytesToPad = 16 - Password.length();

              // Pad with the number of padding bytes
              if (bytesToPad) {
                for (int i = 16 - bytesToPad; i < 16; i++) {
                  Password += (char)bytesToPad;
                }
              }
              // Encryption and decryption process
              String output;
              String decryptedOutput;
              // Encrypt the input using the padded buffer as the key
              encryptStringCBC(Password, Password.c_str(), output);

              // Decrypt the output using the same key
              decryptStringCBC(output, Password.c_str(), decryptedOutput);

              Serial.println("All data entered and processed successfully!");
              //Serial.println("UserID: " + String(NewUser.UserID));  //Asigned by server.
              Serial.println("Username: " + NewUser.Username);
              Serial.println("Name: " + NewUser.Name);
              Serial.println("NCID: " + String(NewUser.NCID));
              Serial.println("Encrypted Password: " + output);
              Serial.println("Decrypted Password: " + decryptedOutput);
              //Process Json here
              StaticJsonDocument<200> doc;
              doc["Username"] = NewUser.Username;
              doc["Name"] = NewUser.Name;
              doc["NCID"] = NewUser.NCID;
              doc["EcPw"] = output;
              String NewUserJson;
              serializeJson(doc, NewUserJson);
              OP_ADMIN_ADDSTUDENT_Wrapper(NewUserJson);
            } else {
              Serial.println("Input length is too long, must be 16 characters or less.");
            }
            stage = 0;  // Reset stage
            DesiredOperation = 0;
            break;
        }
        break;
      case OP_ADMIN_DELSTUDENT:
        switch (stage) {
          case 0:
            Serial.println("Enter the Username of the user you wish to delete:");
            stage++;
            break;
          case 1:
            Username = input;
            OP_ADMIN_DELSTUDENT_Wrapper(Username);
            stage = 0;  // Reset stage
            DesiredOperation = 0;
            break;
        }
        break;
      case OP_ADMIN_RECORD_DUMP:
        OP_ADMIN_RECORD_DUMP_Wrapper();
        DesiredOperation = 0;
        break;
      case OP_ANDROID_AUTH:
        switch (stage) {
          case 0:
            Serial.println("Please enter the following details:");
            Serial.println("Enter Username:");
            stage++;
            break;
          case 1:
            Username = input;
            Serial.println("Enter Password:");
            stage++;
            break;
          case 2:
            Password = input;
            OP_ANDROID_AUTH_Wrapper(Username, Password);
            stage = 0;  // Reset stage
            DesiredOperation = 0;
            break;
        }
        break;
      case OP_ANDROID_LAYOUT_INFO:  //ill be back.
        DesiredOperation = 0;
        break;
      case OP_ANDROID_LAYOUT:
        DesiredOperation = 0;
        break;
    }
  }
}
