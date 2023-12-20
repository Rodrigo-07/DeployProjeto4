// const { create } = require("domain");
var southWest = L.latLng(-90, -180); // limite à sudoeste do mapa
var northEast = L.latLng(90, 180);   // limite à nordeste do mapa
var bounds = L.latLngBounds(southWest, northEast); // definição dos limites do mapa

// Inicializa o mapa, com as suas devidas configurações
var map = L.map('map', {
    center: [0,0],
    zoom: 2,
    maxBounds: bounds,
    minZoom: 1
})

const button1 = document.getElementById('idSelect');
const button2 = document.getElementById('NE Type');
const button3 = document.getElementById('Data de vencimento');
const button4 = document.getElementById('Board Type');
const button5 = document.getElementById('Localização');

// Add a tile layer (you can use other tile providers)
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    maxZoom: 18,
}).addTo(map);

var markers = []; // Lista para armazenar os dados dos marcadores no mapa
var markersList = []; // Lista para armazenar os marcadores no mapa 
var number = 0;
const buttons = ["NE Type", "Processo", "Projeto", "Board Type", "Data de vencimento", "Localização"];


var selectBox = document.getElementById("idSelect");

function updateMap() {
    const urlParams = new URLSearchParams(window.location.search);
    const userId = urlParams.get('user_id');

    if(userId){
    fetch('/gps-data')
        .then(response => response.json())
        .then(data => {
            data.forEach(row => {
                var lat = row.Latitude;
                var long = row.Longitude;
                var id = row.id;
                var ativo = row.Ativo;
                var date = row.Date;

                // Adicionar o ponto na lista de marcadores
                markers.push(row);  


                //Verifica se o marcador já existe no mapa
                for(var i = 0; i < markers.length; i++){
                    if(markers[i].id != id){
                        var marker = L.marker([lat, long]).addTo(map);

                        // marker.bindPopup("<b>Id: </b>" + String(id) + "<br><b>Ativo: </b>" + String(ativo) + "<br><b>Data: </b>" + String(date) + "<a></a>");
                        marker.bindPopup(`<b>Id: </b> ${String(id)} <br><b>Ativo: </b> ${String(ativo)} <br><b>Data: </b> ${String(date)}` + `<br><a href=https://www.google.com/maps?q=${lat},${long}>Acessar </a>`);
                        markersList.push(marker);
                    }
                }


            });
        })
        .catch(error => console.error('Erro fetching data do GPS:', error));
    } else {
        console.log("User not found");
        clearMarkers();
    }
}

function updateButtons() {
    fetch('/equipment-id')
        .then(response => response.json())
        .then(data => {
            data.forEach(column => {
                createOption(column.Board_Full_Name, button1);
                createOption(column.NE_Type, button2);
                createOption(column.due_date, button3);
                createOption(column.Board_Type, button4);
                createOption(column.UF, button5)
            });
        })
        .catch(error => console.error('Error fetching gps data:', error));
}


function updateESP() {

    fetch('/gps-esp')
            .then(response => response.json())
            .then(data1 => {
                // Fetch data from endpoint2
                fetch('/equipment-esp')
                    .then(response => response.json())
                    .then(data2 => {

                        const ESP1 = data1.map(item => item.ESP);
                        const ESP2 = data2.map(item => item.ESP);
                        // Compare data1 and data2
                        const result = ESP1.filter(item => !ESP2.includes(item));
                        console.log(result);

                        const selectBox = document.getElementById('esp');

                        result.forEach(item => {
                            console.log(item);
                            const newOption = document.createElement("option");
                            newOption.value = item;
                            newOption.textContent = item;
                            selectBox.appendChild(newOption);
                        });
                    })
                        
                .catch(error => console.error('Error fetching id data:', error));
            })
            .catch(error => console.error('Error fetching id data:', error));
            
            
            //data.forEach(column => {
            //    console.log(column.ESP);
           //     const newOption = document.createElement("option");
          //      newOption.value = column.ESP;
          //      newOption.textContent = column.ESP;
         //       selectBox.appendChild(newOption);
         //   });
        
        
        
}
// function toggleButtons() {
//     if (selectBox.selectedIndex != 0){
        
//     }
//     else {
        
//     }
// }

function hideButtons() {
    for (let i = 0; i < buttons.length; i++) {
        var element = document.getElementById(buttons[i]);
        element.classList.add("hidden");
}
}


function resetButton() { //Botão para limpar os filtros     
    button1.selectedIndex = 0;
    button2.selectedIndex = 0;
    button3.selectedIndex = 0;
    button4.selectedIndex = 0;
    button5.selectedIndex = 0;

    clearMarkers();
    searchId();
}

function searchId() { //Pega o valor do filtro de ID e pesquisa os ativos correspondentes
    fetch('/equipment-id') 
        .then(response =>response.json())
        .then(data => {
            const selectBox = document.getElementById("idSelect");
            var selectedOption = selectBox.value;
            if (selectedOption != "") {
                clearMarkers();
            }
            data.forEach(column => {
                if (column.Board_Full_Name == selectedOption) {
                    var lat = column.Latitude;
                    var long = column.Longitude;
                    var id = column.id;
                    var ativo = column.Ativo;
                    var date = column.Date;


                    var marker = L.marker([lat, long]).addTo(map);
                    marker.bindPopup(`<b>Id: </b> ${String(id)} <br><b>Ativo: </b> ${String(ativo)} <br><b>Data: </b> ${String(date)}` + `<br><a href=https://www.google.com/maps?q=${lat},${long}>Acessar </a>`);
                    markersList.push(marker);
                }
            })
        })
    }

function clearMarkers() { //Limpa todos os marcadores do mapa
    for (let i = 0; i < markersList.length; i++) {
        map.removeLayer(markersList[i]);
    }
}

// Função cookies para manter user_id

  
function getUserIDAndNavigate(url) {
    // Função utilitária para pegar o user_id do cookie
    const getUserIdFromCookie = () => {
        const cookieValue = document.cookie.split('; ').find(row => row.startsWith('user_id='));
        return cookieValue ? cookieValue.split('=')[1] : null;
    };
  
    // Função utilitária para adicionar o user_id à query string
    const addUserIdToQueryString = (url, userId) => {
        return userId ? `${url}${url.includes('?') ? '&' : '?'}user_id=${userId}` : url;
    };
  
    // Pegar o user_id do cookie
    const userId = getUserIdFromCookie();
  
    // Adicionar user_id para a query string e navegar para a nova URL
    const newURL = addUserIdToQueryString(url, userId);
    window.location.href = newURL;
  }
  
  

function createOption(value, element) {
    if (verifyOption(element, value) && value != undefined) { //Caso não haja duplicatas, cria uma nova opção com o valor de "value" para o filtro de id "element"
        var newOption = document.createElement("option");
        newOption.value = value;
        newOption.textContent = value;
        element.appendChild(newOption);
    } 
}

//Verifica todas as opções do filtro com base no seu id "select", para evitar opções duplicatas
function verifyOption(select, value) {
    for (let i = 0; i < select.options.length; i++) {
        if (select.options[i].value === value) {
            return false;
        }
    }
    return true;
}