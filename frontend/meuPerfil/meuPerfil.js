const urlParams = new URLSearchParams(window.location.search);
const userId = urlParams.get('user_id');


const apiEndpoint = `/profile/${userId}`;

// Fetch data from the API
fetch(apiEndpoint)
    .then(response => response.json())
    .then(data => {
        data.forEach(row => {

            document.getElementById('name').value = row.nome;
            document.getElementById('empresa').value = row.login;
            document.getElementById('cargo').value = row.setor;
            document.getElementById('projeto').value = row.cargo;
        })
        // Populate the input fields with the fetched data

  
    })
    .catch(error => console.error('Error fetching data:', error));


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
  