//This example shows how to send JSON payload FCM to a recipient via HTTPv1 API.
//This new API requires OAuth2.0 authentication.

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

/* 2. Define the Firebase project host name (required) */
#define FIREBASE_HOST ""

/** 3. Define the Service Account credentials (required for token generation)
*
* This information can be taken from the service account JSON file.
*
* To download service account file, from the Firebase console, goto project settings,
* select "Service accounts" tab and click at "Generate new private key" button
*/
#define FIREBASE_PROJECT_ID ""
#define FIREBASE_CLIENT_EMAIL "";

const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCRPZXNiQGWOm62\npCDrOfz8DXjdk3Iye0uUXkbxr8GmXS20e4zOjwga9tBpVcer0KioSNRcJsEAWVD0\nBJzk9iN6Fv6cTLQWAzvBxuvI5YpetzXMoFxJTxXZmrV5djxwSErFNBbj5xj2FCOa\nEJSjvV1Kf49H58ZCKyrPjP0GX517a5tz5toYHLWCnsUHEbio/pJ9cyIh/bzcp8N0\nNDg73/kcyIawp+90Yv3CLu2KZHCLLdn4zlaZjpudrLwMMHbBgDiri6Z+xc7VLJbH\nTnp5bWmcaXeqEXMMdfQ9GFPG3vX0OpdLIkDIFzaHcTBA1t5O2A4P/prGYbB1kLRf\nDmMXcntjAgMBAAECggEALiQ/FbubffMVXj/ZV3GmPhs3lUk1A/Woprum1TyNjIFY\ntlhz/OFy5UIZJwXB+ANDuabdlGMZeLdnYQagI5tQ3QY2IEi8A6xGTUMZgErwQTP6\nSwZ9GysdWXLtiWd3PwKwgmMQjJnME82FG/p9hmFwJG1PAzyOf1IAeUyky+QPf3hP\nLDfKTmX3zn+pBgSnfMwEUVBEcNiSTXr6nZh6adewIiKNVVfC8azjXveY8UHkAZyS\ndFkEXzMmV1PAGuI2tN/+rJ8i+RHvAzNWE5p+wGMO2PEGPWknfI2V6JuC+aP8docl\nJCfDedTqgOG0rQdtYWzrxjiIuNYhn9KPjnxvxFauHQKBgQDF/hMbpX8RIa4ZCCAf\nQS2cHYUicRoHVw8Bp7iODvKa2hbilihoHAgYq4Z3g+O225+gorwyF03DJt7Zg3NX\nfcknrRnEMFpf7m8vgBEYBBjJ/BLzK64I3n+AgY3HWoXeIgI87IuHO4tPwkLBd2YC\n/UJcD8LqL5zBvKic2LviukUudQKBgQC7yvyaEaZG+DgYuTN5A3D4LZjBsGwPG1W7\nkGGSzDPtOSzDXcWwmozjW0rWZl+cMEufykIccLN7m7Ir6AT2zHbI1f6npdLq+v+p\nLuylwnT205f4hyAqVp+PSm3WXF0/aBbs54m9MxqJzLwD/aPzu72z4WE5BbB/bF7b\nOHEzSrv3dwKBgHIHEhoqBjRnoUzpURavAwWWOsbLPKJdkVbysWefVDm6wNNeh4bu\nHfuXTaonW05EznpFPv3aTf7GtDGxcwxAdUlUeDJ1Zpc5Su+gYjfipCQv+Pnzfhe9\njNEFzBG0IMT98SwFT5ZixKj6yFeD3Ex8b2t16FvXcABkFve+GvdJL+7BAoGAJazH\nmLFJxpstxegB55lFJMUhDFq0kyNMw+Bncuf80u7RiDBb5N5cvZAnJ3RKYi9ePKR7\nNvF1UKsfx6c4t7q1af1kpYWjFvlcKTj8yRSGGq7Aol+ZmGTghgLMnmDRZFMhpk2d\nWJPUEyk73yEXr2JO4LOqqaH2ktnO/fQI4d1wcGkCgYA8K3U1emEoEJEqk3PaWlLG\ng6YZO6niB0Fabiy6rOee0y9Pm3SrtTO6oL/ayckaSqX6rX7hrOqOIOEHllWIag1e\nU9wRdUT66ZELsvfiH3Rs7msVvb7zNy8J3sFysVhUErGevVR88vSTzPQ/CliprdvX\nTPA6Hl0/onWwG7o+XGmv2w==\n-----END PRIVATE KEY-----\n";
/* 4. Define the ID token for client or device to send the message */
#define DEVICE_REGISTRATION_ID_TOKEN "//aqui va el token"

/* 5. Define the Firebase Data object */
FirebaseData fbdo;

/* 6. Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;

/* 7. Define the FirebaseConfig data for config data */
FirebaseConfig config;

unsigned long lastTime = 0;

int count = 0;

void sendMessage();
int contador = 0;
void setup()
{

Serial.begin(115200);

WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
Serial.print("Connecting to Wi-Fi");
while (WiFi.status() != WL_CONNECTED)
{
Serial.print(".");
delay(300);
}
Serial.println();
Serial.print("Connected with IP: ");
Serial.println(WiFi.localIP());
Serial.println();

/* Assign the project host (required) */
config.host = FIREBASE_HOST;

/* Assign the sevice account credentials and private key (required) */
config.service_account.data.client_email = FIREBASE_CLIENT_EMAIL;
config.service_account.data.project_id = FIREBASE_PROJECT_ID;
config.service_account.data.private_key = PRIVATE_KEY;

Firebase.begin(&config, &auth);

Firebase.reconnectWiFi(true);

#if defined(ESP8266)
//Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
fbdo.setBSSLBufferSize(1024, 1024);
#endif

//Set the size of HTTP response buffers in the case where we want to work with large data.
fbdo.setResponseSize(1024);

//sendMessage();
}

void loop()
{

//Verificar que la conexión a firebase exista


if(Firebase.ready()){


//Obtener un valor entero desde Firebase
if(Firebase.RTDB.getString(&fbdo,"/test/relay")){//Colección
Serial.print("Obteniendo Valor Entero, int = ");
Serial.println(fbdo.stringData());

if(1 == fbdo.stringData().toInt()) {
digitalWrite(2,HIGH);//CONECTAR UN LED EN D4
}
else {
digitalWrite(2,LOW);
}
}
else{
Serial.print("Error: ");
Serial.println(fbdo.errorReason());
}



/******************Subir datos desde el ESP8266***************************/
if(Firebase.RTDB.setInt(&fbdo,"/firebase/int",contador)){
contador = contador +=1;
Serial.println(contador);
Serial.println("Valor guardado en Firebase");

}
else{
Serial.print("Error subiendo datos: ");
Serial.println(fbdo.errorReason());
}
if(Firebase.RTDB.setInt(&fbdo,"/firebase/float",contador)){
contador = contador +=2;
Serial.println(contador);
Serial.println("Valor float guardado en Firebase");

}
else{
Serial.print("Error subiendo datos: ");
Serial.println(fbdo.errorReason());
}
delay(3000);
}



if (millis() - lastTime > 60 * 500)
{
    lastTime = millis();

    sendMessage();
}
}

void sendMessage()
{

Serial.println("------------------------------------");
Serial.println("Send Firebase Cloud Messaging...");

//Read more details about HTTP v1 API here https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages
FCM_HTTPv1_JSON_Message msg;

msg.token = DEVICE_REGISTRATION_ID_TOKEN;

msg.notification.body = "Hola desde un Esp8266";
msg.notification.title = "Esto usa FCM";

FirebaseJson json;
String payload;

//all data key-values should be string
//json.add("temp", "28");
//json.add("unit", "celsius");
json.add("data", "data");
json.toString(payload);
msg.data = payload.c_str();

if (Firebase.FCM.send(&fbdo, &msg)) //send message to recipient
{

Serial.println("PASSED");
Serial.println(Firebase.FCM.payload(&fbdo));
Serial.println("------------------------------------");
Serial.println();
}
else
{
Serial.println("FAILED");
Serial.println("REASON: " + fbdo.errorReason());
Serial.println("------------------------------------");
Serial.println();
}

count++;
}