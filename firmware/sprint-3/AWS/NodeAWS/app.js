const mqtt = require('mqtt');
const fs = require('fs');

// Replace these values with your AWS IoT settings
const iotEndpoint = 'a4alqaphohg6o-ats.iot.us-east-2.amazonaws.com';
const clientId = 'ESP_32';
const certPath = './2237c0d9a77e691d041951a241e19e5519a75bfc3a1b8b325fb22871ba8a5d32-certificate.pem.crt';
const keyPath = './2237c0d9a77e691d041951a241e19e5519a75bfc3a1b8b325fb22871ba8a5d32-private.pem.key';
const caPath = './AmazonRootCA1.pem';
const topic = 'esp32/pub';

// Create the client connection
const client = mqtt.connect({
  host: iotEndpoint,
  port: 8883,
  clientId,
  cert: fs.readFileSync(certPath),
  key: fs.readFileSync(keyPath),
  ca: fs.readFileSync(caPath),
  protocol: 'mqtts',
});

// Handle connection events
client.on('connect', () => {
  console.log('Connected to AWS IoT broker');

  // Subscribe to the desired topic
  client.subscribe(topic, (err) => {
    if (!err) {
      console.log(`Subscribed to ${topic}`);
    }
  });
});

// Handle incoming messages
client.on('message', (topic, message) => {
    try {
      const payload = JSON.parse(message.toString());
      console.log(`Received message on topic ${topic}:`);
      console.log(payload);
    } catch (error) {
      console.error(`Error parsing message: ${error}`);
    }
  });
  
// Handle errors
client.on('error', (error) => {
  console.error(`Error: ${error}`);
});

// Handle disconnection
client.on('close', () => {
  console.log('Connection closed');
});

// Handle graceful shutdown
process.on('SIGINT', () => {
  console.log('Disconnecting from AWS IoT broker');
  client.end(() => {
    process.exit();
  });
});
