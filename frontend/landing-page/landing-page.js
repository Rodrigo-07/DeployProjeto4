
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

