var username = false;

function validateData() {
    cleanMessage();

    var emailID = document.getElementById("user")
    var emailValue = emailID.value;

    returnMessage = validateEmail(emailValue)
        
    if (returnMessage == "Correto") {
        emailID.classList.remove("inputField");
        validateLogin();
    }
    if (returnMessage == "Por favor, preencha esse campo") {
        showMessage("user", returnMessage);
    }
    if (returnMessage == "Email inválido, tente novamente") {
        showMessage("user", returnMessage);

    }
}

function validateEmail (email) {
    const padraoEmail = /^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9-]+(?:\.[a-zA-Z0-9-]+)*$/;

    if (email == "") {
        return "Por favor, preencha esse campo"
    }
    if (padraoEmail.test(email) == false) {
        return "Email inválido, tente novamente"
    }
    return "Correto"
}

function showMessage(fieldID, message){
	var inputField = document.getElementById(fieldID);
	inputField.classList.add('inputField');
	inputField.focus();

	var fieldMessage = document.createElement("p");
	fieldMessage.innerHTML = message;
	fieldMessage.classList.add("field-message");

	inputField.parentNode.insertBefore(fieldMessage, inputField.nextSibling);	
}

function cleanMessage() {
    var elements = document.getElementsByClassName("field-message")
    while (elements.length > 0) {
        elements[0].parentNode.removeChild(elements[0]);
    }
}

function validateLogin() {
    fetch('/user-authentication')
        .then(response => response.json())
        .then(data => {
            console.log("início");
            var login = document.getElementById("user").value;
            var password = document.getElementById("password").value;

            data.forEach(column => {
                if (column.login == login) {
                    console.log(password);
                    // Verifica se a senha é igual ao do banco de dados
                    if (column.password == password) {
                        // Set a cookie with the user_id
                        document.cookie = `user_id=${encodeURIComponent(column.ID)}; expires=Thu, 01 Jan 2099 00:00:00 UTC; path=/`;

                        // Call the sendLogin function if needed
                        sendLogin();

                        // Use window.location.href to navigate to the landing page
                        const newURL = `../landing-page/landing-page.html${column.ID ? `?user_id=${column.ID}` : ''}`;
                        window.location.href = newURL;

                    } else {
                        console.log("Senha incorreta");
                    }
                } else {
                    console.log("Email não cadastrado");
                }
            })
        })
}

function sendLogin() {
    fetch('/receberLogin', {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json',

    },
    body: JSON.stringify({ username: username })
    }
    )
    .then(response => response.json())
    .then(data => {
        console.log(data);
        })
    .catch(error => {
        console.error('Erro na solicitação', error);
        });
    }
