
const urlParams = new URLSearchParams(window.location.search); // Pegar os parametros da URL
const USER_ID = urlParams.get('user_id') // Salvar o parametro user_id na variável USER_ID

document.getElementById('placa').addEventListener('input', function() {
  const searchInput = this.value;
  searchAtivosOnServer(searchInput, USER_ID); // Executar a função SearchAtivosOnServer com o parametro searchInput e USER_ID
});

// Função para buscar os ativos no servidor
async function searchAtivosOnServer(searchTerm, userId) {
  try {
    const response = await fetch(`http://localhost:3000/search?term=${searchTerm}&userId=${userId}`); // Fazer o fetch dos dados
    if (response.ok) { // Se a resposta for ok
      const data = await response.json(); // Salvar os dados em data
      displaySearchResults(data); // Executar a função displaySearchResults com o parametro data
      console.log(data); // Mostrar os dados no console
    } else {
      throw new Error('Server responded with an error');
    }
  } catch (error) {
    console.error(`Error searching Ativos on the server: ${error.message}`);
  }
}

// Função para mostrar os resultados da busca
function displaySearchResults(results) {
  const container = document.getElementById('searchResults');
  container.innerHTML = '';

  results.forEach(result => {
    const resultDiv = document.createElement('div');
    resultDiv.classList.add('container', 'pb-2', 'mt-5', 'd-flex', 'flex-column', 'justify-content-center');
    resultDiv.style.backgroundColor = 'rgba(228, 228, 228, 0.70)';
    resultDiv.style.borderRadius = '50px';
    resultDiv.style.width = '1050px';

    const row = document.createElement('div');
    row.classList.add('row', 'align-items-center', 'mt-2');

    const resultLink = document.createElement('a');
    resultLink.classList.add('col-8', 'text-center');

    // Adicionar estilo ao link
    resultLink.style.fontSize = '24px';
    resultLink.style.color = '#6038F2';
    resultLink.style.fontWeight = 'bold';
    resultLink.style.textDecoration = 'none'; 
    resultLink.style.cursor = 'pointer'; 
    
    // // Adicionar o evento de click ao link
    resultLink.onclick = function() {
      const redirectURL = `/dashboard/dashboard.html?ativo=${encodeURIComponent(result.Board_Full_Name)}&user_id=${USER_ID}`;
      window.location.href = redirectURL;
    };

    resultLink.textContent = result.Board_Full_Name;

    const removeButton = createRemoveButton(result.Ativo, result.Board_Full_Name, USER_ID);
    removeButton.classList.add('col-4', 'text-center', 'ml-2');

    row.appendChild(resultLink);
    row.appendChild(removeButton);

    resultDiv.appendChild(row);

    container.appendChild(resultDiv);
  });
}

// Função para criar o botão de remover
function createRemoveButton(ativoId, boardFullName, userId) { 
  const removeButton = document.createElement('button'); // Criar o botão
  removeButton.classList.add('button'); // Adicionar a classe
  removeButton.style.marginLeft = '10px'; // Adicionar a margem
  removeButton.textContent = 'Remover dos Favoritos'; // Adicionar o texto

  removeButton.addEventListener('click', async () => { // Adicionar o evento de click
    try {
      const ativoId = await getAtivoIdByBoardFullName(boardFullName); // Pegar o ativoId pelo boardFullName
      if (ativoId !== undefined) { // Se o ativoId for diferente de undefined
        await removeFromFavoritos(ativoId, userId); // Remover o ativo dos favoritos
      } else {
        throw new Error('Ativo ID not found for Board_Full_Name'); // Se não, mostrar o erro
      }
    } catch (error) {
      console.error(`Error removing Ativo from Favoritos: ${error.message}`); // Mostrar o erro no console
    }
  });

  return removeButton;
}

// Função para pegar o ativoId pelo boardFullName
async function getAtivoIdByBoardFullName(boardFullName) {
  try {
    const response = await fetch('http://localhost:3000/getAtivoId', { // Fazer o fetch dos dados
      method: 'POST', // Método POST
      headers: {
        'Content-Type': 'application/json', // Tipo de conteúdo
      },
      body: JSON.stringify({ boardFullName }), // Passar o boardFullName
    });

    if (response.ok) { // Se a resposta for ok
      const data = await response.json();
      return data.ativoId;
    } else {
      throw new Error('Server responded with an error');
    }
  } catch (error) { // Se não, mostrar o erro no console
    console.error(`Error getting Ativo ID by Board_Full_Name: ${error.message}`);
    return undefined;
  }
}

// Função para remover o ativo dos favoritos
async function removeFromFavoritos(actvId, userId) {
  try {
    const response = await fetch('http://localhost:3000/deleteFavorite', { // Fazer o fetch dos dados
      method: 'DELETE', // Método DELETE
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({ userId, actvId }), // Passar o userId e o actvId
    });

    if (!response.ok) {
      throw new Error('Server responded with an error');
    }
  
    console.log(`Ativo ${actvId} removed from Favoritos for user ${userId}`);
    var feedbackModal = new bootstrap.Modal(document.getElementById('feedbackModalRFAV'));
      feedbackModal.show();
      
  } catch (error) {
    console.error(`Error removing Ativo ${actvId} from Favoritos: ${error.message}`);
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


  
  