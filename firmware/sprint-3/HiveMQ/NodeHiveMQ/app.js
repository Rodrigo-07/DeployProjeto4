const mqtt = require('mqtt');
const fs = require('fs');
const sqlite3 = require('sqlite3').verbose();
const express = require('express');

const app = express();
const portApp = 3000;


// Serve static files (e.g., HTML, CSS)
app.use(express.static('public'));

// Endpoint to fetch GPS data
app.get('/gps-data', (req, res) => {
    const sql = 'SELECT * FROM gps_data';

    dbConnection.all(sql, (err, rows) => {
        if (err) {
            console.error('Error fetching GPS data from the database:', err);
            res.status(500).send('Internal Server Error');
        } else {
            res.json(rows);
        }
    });
});

app.listen(portApp, () => {
    console.log(`Server running at http://localhost:${portApp}`);
});

// Definação dos parametros de conexão MQTT
const mqtt_server = "3d23a9722fe5468cbb05f9e25035c21e.s2.eu.hivemq.cloud"; // Host do HiveMQ broker
const port = 8883; // Porta de conexão
// Credencias de conexão geradas no HiveMQ
const mqtt_user = "ESP32_Connect4";
const mqtt_pass = "123drigo";

const dbFilePath = './GPS.db';

// Create a SQLite database connection
const dbConnection = new sqlite3.Database(dbFilePath);

// Topicos de conexeão
const topiGPS = "GPS";
const topicLat = "GPS/Latitude";
const topicLong = "GPS/Longitude";
const caFile = './ca_certificate.pem'; // Endereço do cerificado CA

// Parametros de conexão MQTT
const options = {
  clientId: 'mqttjs_' + Math.random().toString(16).substr(2, 8),
  username: mqtt_user,
  password: mqtt_pass,
  port: port,
  protocol: 'mqtts', // MQTT over TLS
  ca: [fs.readFileSync(caFile)], // Certificado CA
};

// Conexão MQTT
const client = mqtt.connect(`mqtt://${mqtt_server}`, options);

client.on('connect', () => {

  console.log('Connected to MQTT broker');

  // Inscrição nos topicos para receber mensagems que estão sendo publicadas pelo esp32
  client.subscribe(topicLat);
  client.subscribe(topicLong);
  client.subscribe(topiGPS);
});

// Caputrar erros
client.on('error', (err) => {
  console.error('Error:', err);
});

// Formartar mensagems recebidas
client.on('message', (topic, message) => {
  console.log(`Received message on topic ${topic}: ${message}`);


  const gpsData = JSON.parse(message.toString());

  // Insert the GPS data into the SQLite database
  const sql = 'INSERT INTO gps_data (Latitude, Longitude, Date, Ativo) VALUES (?, ?, ?, ?)';
  const values = [gpsData.Latitude, gpsData.Longitude, gpsData.Date, gpsData.Ativo];

  dbConnection.run(sql, values, (err) => {
      if (err) {
          console.error('Error inserting data into the database:', err);
      } else {
          console.log('Data inserted into the database');
      }
  });

});

client.subscribe('Teste');

// Desconexão com o broker
process.on('SIGINT', () => {
  console.log('Disconnecting from MQTT broker');
  client.end(() => {
    process.exit();
  });
});
