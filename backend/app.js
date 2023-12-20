const mqtt = require('mqtt');
const fs = require('fs');
const path = require('path');
const sqlite3 = require('sqlite3').verbose();
const express = require('express');
const bodyParser = require('body-parser');

var username = false;

const app = express();
app.use(express.json());

const portApp = 3000;

// !!!!!!!!!!!!!!!! ENDPOINT PÁGINA WEB !!!!!!!!!!!

app.use(bodyParser.json());

// Carregar as paginas HTML e CSS
const frontendPath = path.join(__dirname, '..', 'frontend');
app.use(express.static(frontendPath));

app.post('/receberLogin', (req, res) => {
  username = req.body.username;
  res.json({ mensagem: 'username' });
});

// Endpoint para coletar todosos  dados do GPS
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


app.post('/add-equipment', (req, res) => {
  const data = req.body;

  const query = `INSERT INTO info_equipment (NE_Name, UF, Cidade, Board_Full_Name, Board_Name, Board_Type, NE_Type, SN, PN, ESP, State) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)`;
  dbConnection.run(query, [data.ne_name, data.uf, data.cidade, data.board_full_name, data.board_name, data.board_type, data.ne_type, data.sn, data.pn, data.esp, data.state], function(err) {
      if (err) {
          return console.error(err.message);
      }
      res.json({ message: 'Data saved successfully', id: this.lastID });
  });
});

app.post('/add-user', (req, res) => {
  const data = req.body;

  const query = `INSERT INTO user_authentication (login, password, nome, setor, cargo) VALUES (?, ?, ?, ?, ?)`;
  dbConnection.run(query, [data.login, data.password, data.nome, data.setor, data.cargo], function(err) {
      if (err) {
          return console.error(err.message);
      }
      res.json({ message: 'Data saved successfully', id: this.lastID });
  });
});

app.get('/equipment-info', (req, res) => {
  const sql = 'SELECT * FROM info_equipment';

  dbConnection.all(sql, (err, rows) => {
    if (err) {
      console.error('Error fetching equipment info from the database', err);
      res.status(500).send('Internal Server Error');
    } else {
      res.json(rows);
    }
  });
});

app.get('/equipment-id', (req, res) => {
  const sql = 'SELECT * FROM gps_data INNER JOIN info_equipment ON gps_data.ESP = info_equipment.ESP';

  dbConnection.all(sql, (err, rows) => {
    if (err) {
      console.error('Error fetching equipment id data from the database:', err);
      res.status(500).send('Internal Server Error');
    } else {
      res.json(rows);
    }
  });
});

app.get('/equipment-esp', (req, res) => {
  const sql = 'SELECT ESP FROM info_equipment';

  dbConnection.all(sql, (err, rows) => {
    if (err) {
      console.error('Error fetching equipment id data from the database:', err);
      res.status(500).send('Internal Server Error');
    } else {
      res.json(rows);
    }
  });
});

app.get('/gps-esp', (req, res) => {
  const sql = 'SELECT ESP FROM gps_data';

  dbConnection.all(sql, (err, rows) => {
    if (err) {
      console.error('Error fetching equipment id data from the database:', err);
      res.status(500).send('Internal Server Error');
    } else {
      res.json(rows);
    }
  });
});

app.get('/user-authentication', (req, res) => {
  const sql = 'SELECT * FROM user_authentication';

  dbConnection.all(sql, (err, rows) => {
    if(err) {
      console.error("Error fetching data from user_authentication:", err);
      res.status(500).send('Internal Server Error');
    } else {
      res.json(rows);
    }
  })
})

app.get('/profile/:id', (req, res) => {
const user_id = req.params.id

  const sql = 'SELECT * FROM user_authentication WHERE ID = ?';

  dbConnection.all(sql, [user_id],(err, rows) => {
    if(err) {
      console.error("Error fetching data from user_authentication:", err);
      res.status(500).send('Internal Server Error');
    } else {
      res.json(rows);
    }
  })
})

app.get('/id-active', (req, res) => {
  const sql = 'SELECT Ativo FROM gps_data';

  dbConnection.all(sql, (err, rows) => {
    if(err) {
      console.error("Error fetching data:", err);
      res.status(500).send('Internal Server Error');
    } else {
      res.json(rows);
    }
  });
});

app.get('/getGPSData/:boardFullName', (req, res) => {
  const boardFullName = req.params.boardFullName;

  const sql = `
    SELECT 
        gd.Latitude, gd.Longitude, gd.Date, gd.Ativo, ie.ESP
    FROM 
        info_equipment ie 
    JOIN 
        gps_data gd ON ie.ESP = gd.ESP 
    WHERE 
        ie.Board_Full_Name = ?;
  `;

  dbConnection.all(sql, [boardFullName], (err, details) => {
    if (err) {
      console.error('Error fetching data:', err);
      res.status(500).send('Internal Server Error');
    } else {
      res.json(details);
    }
  });
});


app.get('/details/:ativoName', (req, res) => {
  const ativoName = req.params.ativoName;

  const sql = 'SELECT Latitude, Longitude, Date FROM gps_data WHERE Ativo = ?';
  dbConnection.all(sql, [ativoName], (err, details) => {
    if(err) {
      console.error('Erro fetching data:', err);
      res.status(500).send('Internal Server Error');
    } else {
      res.json(details);
    }
  })
})

app.get('/login', (req, res) => {
  res.sendFile(path.join(frontendPath, '/login/login.html'));
});

// Favoritos

app.get('/search', async (req, res) => {
  const searchTerm = req.query.term;
  const userId = req.query.userId;

  if (!searchTerm || !userId) {
    return res.status(400).json({ error: 'Search term and userId are required' });
  }

  const sqlQuery = `
    SELECT info_equipment.Board_Full_Name
    FROM info_equipment
    JOIN favoritos ON info_equipment.id = favoritos.actvId
    JOIN user_authentication ON user_authentication.ID = favoritos.UsrId
    WHERE favoritos.UsrId = ? AND info_equipment.Board_Full_Name LIKE ?;
  `;

  try {
    const results = await queryDatabase(sqlQuery, [userId, `%${searchTerm}%`]);
    res.status(200).json(results);
  } catch (error) {
    console.error('Error searching Ativos:', error.message);
    res.status(500).json({ error: 'Internal Server Error' });
  }
});

function queryDatabase(sql, params) {
  return new Promise((resolve, reject) => {
    dbConnection.all(sql, params, (err, rows) => {
      if (err) {
        reject(err);
      } else {
        resolve(rows);
      }
    });
  });
}

app.post('/favorite', async (req, res) => {
  const { userId, actvId } = req.body;

  // Checar se favorito ja existe
  const checkSql = `SELECT COUNT(*) as count FROM favoritos WHERE UsrId = ? AND actvId = ?`;

  dbConnection.get(checkSql, [userId, actvId], (checkErr, row) => {
    if (checkErr) {
      return res.status(500).json({ error: 'Internal server error' });
    }

    if (row.count > 0) {
      // Se já existe responder algo
      return res.status(400).json({ error: 'Favorito already exists for this user and active' });
    }

    // Se não exisitr, inserir
    const insertSql = `INSERT INTO favoritos (UsrId, actvId) VALUES (?, ?)`;

    dbConnection.run(insertSql, [userId, actvId], function (err) {
      if (err) {
        return res.status(500).json({ error: 'Internal server error' });
      }

      res.json({ message: 'Data saved successfully', id: this.lastID });
    });
  });
});

app.get('/searchId', async (req, res) => {
  const boardFullName = req.query.boardFullName;

  if (!boardFullName) {
    return res.status(400).json({ error: 'Board_Full_Name is required' });
  }

  const sqlQuery = `
    SELECT id
    FROM info_equipment
    WHERE Board_Full_Name = ?;
  `;

  try {
    const results = await queryDatabase(sqlQuery, [boardFullName]);

    if (results.length > 0) {
      res.status(200).json({ id: results[0].id });
    } else {
      res.status(404).json({ error: 'ID not found for the provided Board_Full_Name' });
    }
  } catch (error) {
    console.error('Error searching for ID:', error.message);
    res.status(500).json({ error: 'Internal Server Error' });
  }
});


app.delete('/deleteFavorite', (req, res) => {
  const { userId, actvId } = req.body;

  const sql = `DELETE FROM favoritos
    WHERE UsrId = ? AND actvId = ?;`;

  dbConnection.run(sql, [userId, actvId], function(err) {
    if (err) {
      return res.status(500).json({ error: err.message });
    }

    res.json({ message: 'Ativo removed from Favoritos', changes: this.changes });
  });
});

app.post('/getAtivoId', (req, res) => {
  const { boardFullName } = req.body;

  const sql = `SELECT id FROM info_equipment WHERE Board_Full_Name = ?`;

  dbConnection.get(sql, [boardFullName], (err, row) => {
    if (err) {
      return res.status(500).json({ error: err.message });
    }

    if (!row) {
      return res.status(404).json({ error: 'AtivoId not found for the given Board_Full_Name' });
    }

    res.json({ ativoId: row.id });
  });
});


// Endpoint carregar página principal
app.get('/', (req, res) => {
  if (username == true) {
    res.sendFile(path.join(frontendPath, '/mainPage/main_page1.html'));
  } else {
    res.redirect('/login/login.html');
  }
});

app.get('/mainPage/main_page1.html', (req, res) => {
  if (username == true) {
    res.sendFile(path.join(frontendPath, '/mainPage/main_page1.html'));
  } else {
    res.sendFile(path.join(frontendPath, '/login/'));
  }
});

// Página de teste para viualizar os dados do GPS em uma tabela
app.get('/table', (req, res) => {
  if (username == true) {
    res.sendFile(path.join(frontendPath, '/table.html'));
  } else {
    res.sendFile(path.join(frontendPath, '/login/'));
  }
});

app.get('/favoritos', (req, res) => {
  if (username == true) {
    res.sendFile(path.join(frontendPath, '/favoritos/favoritos.html'));
  } else {
    res.sendFile(path.join(frontendPath, '/login/'));
  }
});

app.get('/meuPerfil', (req, res) => {
  if (username == true) {
    res.sendFile(path.join(frontendPath, '/meuPerfil/meuPerfil.html'));
  } else {
    res.sendFile(path.join(frontendPath, '/login/'));
  }
});

app.get('/dashboard', (req, res) => {
  if (username == true) {
    res.sendFile(path.join(frontendPath, '/dashboard/dashboard.html'));
  } else {
    res.sendFile(path.join(frontendPath, '/login/'));
  }
});

app.get('/relatorio', (req, res) => {
  if (username == true) {
    res.sendFile(path.join(frontendPath, '/relatorio/relatorio.html'));
  } else {
    res.sendFile(path.join(frontendPath, '/login/'));
  }
});

app.get('/cadastro', (req, res) => {
    res.sendFile(path.join(frontendPath, '/cadastro/cadastro.html'));
});



// !!!!!!!!!!!!!!!! ENDPOINT MQTT !!!!!!!!!!!!!!!!

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
  const sql = 'INSERT INTO gps_data (Latitude, Longitude, Date, Ativo, ESP) VALUES (?, ?, ?, ?, ?)';
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



// Iniciar servidor
app.listen(portApp, () => {
  console.log(`Server running at http://localhost:${portApp}`);
});
