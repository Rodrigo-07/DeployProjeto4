// Função para setar o nome do ativo no dropdown
function setDropdownName(ativoName) {
  const dropdownButton = document.getElementById("dropdownButton"); // Pegar o elemento do dropdown

  if (dropdownButton) {
    // Se o elemento existir
    dropdownButton.textContent = ativoName; // Setar o texto do elemento
  } else {
    // Se não, mostrar o erro no console
    console.error("Error: dropdownButton is null");
  }
}

// Função para atualizar o dropdown
function updateDropdown() {
  fetch("/equipment-info") // Fazer o fetch dos dados
    .then((response) => response.json())
    .then((data) => {
      const dropdown = document.getElementById("dropdownMenu");

      dropdown.innerHTML = "";

      const urlSearchParams = new URLSearchParams(window.location.search); // Pegar os parâmetros da URL
      const ativoParam = urlSearchParams.get("ativo"); // Pegar o ativo da URL

      data.forEach((item) => {
        // Para cada item no data
        const ativoName = item.Board_Full_Name;

        if (
          ativoName !== null &&
          !Array.from(dropdown.children).some(
            (li) => li.textContent === ativoName
          )
        ) {
          // Checar se o nome do ativo é Null e caso seja, não adicionar o mesmo com nome Null
          const li = document.createElement("li");
          const a = document.createElement("a");
          a.className = "dropdown-item";
          a.href = "#";
          a.textContent = ativoName;
          a.setAttribute("data-value", ativoName);

          a.onclick = function () {
            // Ao clicar no ativo
            const detailsUrl = `/details/${ativoName}`;
            const detailsUrlWithUserId = addUserIdToQueryString(detailsUrl);

            fetch(detailsUrlWithUserId) // Fazer o fetch dos dados
              .then((response) => response.json())
              .then((details) => {
                setDropdownName(ativoName);
                const newUrl = `?ativo=${encodeURIComponent(ativoName)}`;
                window.history.pushState({},
                  "",
                  addUserIdToQueryString(newUrl)
                );
              })
              .catch((error) =>
                console.error(`Error fetching details for ${ativoName}:`, error)
              );
          };

          li.appendChild(a);
          dropdown.appendChild(li);

          // Setar o nome do ativo no dropdown se o mesmo estiver na URL
          if (ativoParam && ativoName === ativoParam) {
            setDropdownName(ativoName);
          }
        }
      });
    })
    .catch((error) => console.error("Error fetching Ativo data:", error));
}

// Função utilitária para adicionar o user_id à query string
function addUserIdToQueryString(url) {
  // Pegar o user_id do cookie
  const userId = getUserIdFromCookie();

  // Adicionar user_id para a query string
  return userId ?
    `${url}${url.includes("?") ? "&" : "?"}user_id=${userId}` :
    url;
}

// Função utilitária para pegar o user_id do cookie
function getUserIdFromCookie() {
  const cookieValue = document.cookie
    .split("; ")
    .find((row) => row.startsWith("user_id="));
  return cookieValue ? cookieValue.split("=")[1] : null;
}

// Função para pegar o id do ativo pelo nome
updateDropdown();

const redirectButton = document.getElementById("redirectButton"); // Pegar o elemento do botão

redirectButton.addEventListener("click", function () {
  // Adicionar o evento de click

  // Ler a URL atual
  const currentURL = window.location.href;

  // Pegar os parâmetros da URL
  const urlParams = new URLSearchParams(window.location.search);
  const selectedAtivo = urlParams.get("ativo");

  // Adicionar o ativo selecionado à URL
  const newURL = `../relatorio/relatorio.html${
    selectedAtivo ? `?ativo=${selectedAtivo}` : ""
  }`;

  // Redirecionar para a nova URL
  window.location.href = newURL;
});

// Parte favoritos //

// Função para adicionar o ativo aos favoritos
function addToFavoritesFromURL() {
  // Pegar os parâmetros da URL
  const urlParams = new URLSearchParams(window.location.search);
  const boardFullName = urlParams.get("ativo");
  const userId = urlParams.get("user_id");

  // Checar se tanto o boardFullName quanto o userId estão presentes na URL
  if (boardFullName && userId) {
    // Pesquisar o id do ativo pelo nome
    searchIdByBoardFullName(boardFullName, userId);
  } else {
    console.log("Either 'board_full_name' or 'user_id' is missing in the URL.");
  }
}

function searchIdByBoardFullName(boardFullName, userId) {
  // Função para pesquisar o id do ativo pelo nome

  const searchEndpoint = `http://localhost:3000/searchId?boardFullName=${encodeURIComponent(
    boardFullName
  )}`;

  // Fazer o fetch dos dados
  fetch(searchEndpoint)
    .then((response) => {
      if (!response.ok) {
        throw new Error("Network response was not ok");
      }
      return response.json();
    })
    .then((result) => {
      if (result.id) {
        const actvId = result.id;
        // Adicionar o ativo aos favoritos
        addToFavorites(actvId, userId);
      } else {
        console.log("No matching id found for the provided Board_Full_Name.");
      }
    })
    .catch((error) => {
      console.error("Error searching for id:", error);
    });
}

// Função para adicionar o ativo aos favoritos
function addToFavorites(actvId, userId) {
  const numericUserId = parseInt(userId, 10);

  // Dados a serem enviados no corpo da requisição
  const data = {
    userId: numericUserId,
    actvId: actvId,
  };

  console.log(data);

  // Fazer o fetch dos dados
  fetch("/favorite", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(data),
    })
    .then((response) => {
      if (!response.ok) {
        throw new Error("Network response was not ok");
      }
      return response.json();
    })
    .then((responseData) => {
      console.log("Added to favorites:", responseData);

      // Checar se a resposta do servidor é um erro
      if (responseData.error) {
        // Alertar que o ativo já está nos favoritos
        alert(responseData.error);
      } else {
        // Favorito adicionado com sucesso
        var feedbackModal = new bootstrap.Modal(
          document.getElementById("feedbackModalFAV")
        );
        feedbackModal.show();
      }
    })
    .catch((error) => {
      console.error("Error adding to favorites:", error);

      var feedbackModalERR = new bootstrap.Modal(
        document.getElementById("feedbackModalFAVERR")
      );
      feedbackModalERR.show();
    });
}


function fetchEquipmentInfo(preSelection) {
  // Ajuste a URL para incluir a pré-seleção
  const url = `/equipment-info?preSelection=${encodeURIComponent(preSelection)}`;
  console.log(preSelection);

  let filteredData = [];
  fetch(url)
    .then(response => response.json())
    .then(data => {
      //console.log(data);

      // Aqui, assumindo que você tem um atributo 'boardName' em cada item da resposta
      const boardName = preSelection; // ou substitua pela lógica correta para obter o boardName

      // Filtra os elementos com base no boardName
      for (var i = 0; i < data.length; i++) {
        //console.log(data[i].Board_Full_Name);
        if (data[i].Board_Full_Name === boardName) {
          filteredData.push(data[i]);
        }
      }

      document.getElementById("textoDinamico1").innerHTML = formatData(filteredData);
      //console.log(filteredData);
    })
    .catch(error => console.error('Error fetching equipment info:', error));
}

// Função para formatar os dados em divs HTML
function formatData(data) {
  var html = ""; // Inicializar o HTML

  // Iterar lista de dados
  for (var i = 0; i < data.length; i++) {
    // Abrir div com a classe row
    html += "<div class='row mt-3'>";

    // Abrir segunda div com a classe da coluna esquerda
    html += "<div class='col'>";

    // Iterar sobre as primeiras 5 chaves menos o id
    var keysLeftColumn = Object.keys(data[i]).filter(key => key.toLowerCase() !== "id").slice(0, 6);
    for (var keyIndex = 0; keyIndex < keysLeftColumn.length; keyIndex++) {
      var key = keysLeftColumn[keyIndex];
      // Abrir label de formulario
      html += "<label for='" + key.toLowerCase() + "' class='form-label'>" + key + "</label>";

      // Checar chave esp
      if (key.toLowerCase() === "esp") {
        // Criar o input
        html += "<input disabled type='text' class='form-control' id='" + key.toLowerCase() + "' value='" + data[i][key] + "'>";
      } else {
       
        html += "<input disabled type='text' class='form-control' id='" + key.toLowerCase() + "' value='" + data[i][key] + "'>";
      }
    }

    // Fechar a div coluna
    html += "</div>";

    // Abrir div coluna direita
    html += "<div class='col'>";

    // Iterar sobre ultimos 6 itens menos id
    var keysRightColumn = Object.keys(data[i]).filter(key => key.toLowerCase() !== "id").slice(6);
    for (var keyIndex = 0; keyIndex < keysRightColumn.length; keyIndex++) {
      var key = keysRightColumn[keyIndex];
      // Abrir label
      html += "<label for='" + key.toLowerCase() + "' class='form-label'>" + key + "</label>";

      // Checar chave esp
      if (key.toLowerCase() === "esp") {
        // Criar input
        html += "<input disabled type='text' class='form-control' id='" + key.toLowerCase() + "' value='" + data[i][key] + "'>";
      } else {
        // Criar input
        html += "<input disabled type='text' class='form-control' id='" + key.toLowerCase() + "' value='" + data[i][key] + "'>";
      }
    }

    // Fechar div coluna direita
    html += "</div>";

    // Fecha div linha
    html += "</div>";

    // Adicionar quebra de linha
    html += "<br/>";
  }

  return html;
}


function formatData1(data) {
  var html = "<div class='table-responsive'><table class='table table-bordered'>"; // Inicia a tabela HTML with Bootstrap classes

  // Cabeçalho da tabela
  html += "<thead class='thead-dark'>";
  html += "<tr>";
  html += "<th>ID</th>";
  html += "<th>NE Name</th>";
  html += "<th>UF</th>";
  html += "<th>Cidade</th>";
  html += "<th>Board Full Name</th>";
  html += "<th>Board Name</th>";
  html += "<th>Board Type</th>";
  html += "<th>NE Type</th>";
  html += "<th>PN</th>";
  html += "<th>SN</th>";
  html += "<th>ESP</th>";
  html += "<th>State</th>";
  html += "</tr>";
  html += "</thead>";

  // Corpo da tabela
  html += "<tbody>";

  // Itera sobre os dados e adiciona as linhas à tabela
  for (var i = 0; i < data.length; i++) {
    html += "<tr>";
    html += "<td>" + data[i].id + "</td>";
    html += "<td>" + data[i].NE_Name + "</td>";
    html += "<td>" + data[i].UF + "</td>";
    html += "<td>" + data[i].Cidade + "</td>";
    html += "<td>" + data[i].Board_Full_Name + "</td>";
    html += "<td>" + data[i].Board_Name + "</td>";
    html += "<td>" + data[i].Board_Type + "</td>";
    html += "<td>" + data[i].NE_Type + "</td>";
    html += "<td>" + data[i].PN + "</td>";
    html += "<td>" + data[i].SN + "</td>";
    html += "<td>" + data[i].ESP + "</td>";
    html += "<td>" + data[i].State + "</td>";
    html += "</tr>";
  }

  html += "</tbody>";
  html += "</table></div>"; // Fecha a tabela HTML com classes Bootstrap

  return html;
}


// Adiciona um ouvinte de eventos ao documento para lidar com cliques em elementos <a> dentro de #dropdownMenu
document.addEventListener("click", function (event) {
  const target = event.target;

  // Verifica se o elemento clicado é uma tag <a> dentro de #dropdownMenu
  if (target.tagName === "A" && target.closest("#dropdownMenu")) {
    // Obtém o valor do atributo 'data-value' do elemento clicado
    const selectedValue = target.getAttribute("data-value");

    // Chama a função fetchEquipmentInfo com o valor obtido
    fetchEquipmentInfo(selectedValue);
  }
});

document.addEventListener('DOMContentLoaded', function () {
  fetch('/equipment-info')
    .then(response => response.json())
    .then(data => {
      console.log(data);

      const elemento = document.getElementById("textoDinamico");
      elemento.innerHTML = formatData1(data);
    })
    .catch(error => console.error('Error fetching equipment info:', error));
});