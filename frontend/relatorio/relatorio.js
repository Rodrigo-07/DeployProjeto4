// Iniciar dropdown
function setDropdownName(ativoName) {
  const dropdownButton = document.getElementById('dropdownButton'); // Definir constante para botão do dropdown

  if (dropdownButton) { // Checar se tem algum valor no Dropdown
    dropdownButton.textContent = ativoName; // Setar o nome do botão de dropdown para o ativo selecionado
  } else { // Se o botão de dropdown for Null, retornar erro
    console.error("Error: dropdownButton is null");
  }
}

// Gerar PDF com jsPDF
function generatePDF(ativoName, details) {  
  return new Promise((resolve, reject) => { // Criar uma Promise para retornar o PDF Blob
    const { jsPDF } = window.jspdf; // Pegar o jsPDF da janela
    const doc = new jsPDF(); // Instanciar o jsPDF

    doc.setDisplayMode(); // Modo display PDF como default

    // Adicionar informação PDF
    doc.text(`Informações do ativo ${ativoName}:`, 10, 10); // Título
    details.forEach((entry, index) => { // Informações
      const yPosition = 20 + index * 10;
      doc.text(`${index + 1}. Latitude: ${entry.Latitude}, Longitude: ${entry.Longitude}, Data: ${entry.Date}`, 10, yPosition); // Adicionar dados
    });

    // Salvar PDF como Blob
    const pdfBlob = doc.output('blob'); 

    // Resolve na Promise com o pdfBlob gerado
    resolve(pdfBlob);

    $("#downloadPDF").on("click", function (event) {
      doc.save(`Relatório_${ativoName}.pdf`);
    });
  });
}

// Função popular dropdown

function updateDropdown() {
  return fetch('/equipment-info')
    .then(response => response.json())
    .then(data => {
      const dropdown = document.getElementById('ativoDropdown');
      dropdown.innerHTML = ''; // Limpar itens ja existentes dropdown

      // Iterar dados para adicionar no dropdown
      data.forEach(item => {
        const ativoName = item.Board_Full_Name;

        // Checar se o nome do ativo é Null e caso seja, não adicionar o mesmo com nome Null
        if (ativoName !== null && !Array.from(dropdown.children).some(li => li.textContent === ativoName)) {
          const li = document.createElement('li');
          const a = document.createElement('a');
          a.className = 'dropdown-item';
          a.href = '#';
          a.textContent = ativoName;

          a.onclick = function () {
            // Após cliclar no respectivo ativo, chamar a função de dar display do PDF
            displayPDFForAtivo(ativoName);

            // Atualizar a URL com o respectivo ativo
            window.history.pushState({}, "", `?ativo=${encodeURIComponent(ativoName)}`);
          };

          li.appendChild(a);
          dropdown.appendChild(li);
        }
      });
    })
    // Se der algum erro, retornar o seguinte no console;
    .catch(error => {
      console.error('Error fetching Ativo data:', error);
    });
}

// Função para dar display do PDF ja gerado

function displayPDFForAtivo(ativoName) {
  // Chamar a função setDropdownName com o ativo escolhido
  setDropdownName(ativoName);

  // Fetch no ativo escolhido para trazer as informações da base de dados
  fetch(`/getGPSData/${ativoName}`)
    .then(response => response.json())
    .then(details => {
      // Chamar a função para gerar o PDF
      generatePDF(ativoName, details)
        .then(pdfBlob => {
          // Mostrar os dados do PDF na div "dataDisplay"
          const dataDisplayDiv = document.getElementById('dataDisplay');
          const pdfObject = URL.createObjectURL(pdfBlob);
          dataDisplayDiv.innerHTML = `<iframe src="${pdfObject}" style="width: 100%; height: 100%; border: none;"></iframe>`;
        })
        // Se der algum erro gerando o PDF retornar o seguinte no console
        .catch(error => console.error('Error generating PDF:', error));
    // Se der algum erro no fetch retornar o seguinte no console
    })
    .catch(error => console.error(`Error fetching details for ${ativoName}:`, error));
}

// Chamar a função para popular o dropdown de acordo com o que está na URL
const urlParams = new URLSearchParams(window.location.search);
const ativoQueryParam = urlParams.get('ativo');
updateDropdown().then(() => {
  if (ativoQueryParam) {
    // Mostrar o PDF para o seguinte ativo encontrado na URL
    displayPDFForAtivo(ativoQueryParam);
  }
});


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

