//Pensar em uma forma de selecionar um determinado ativo e mostrar as informações


// Faz a requisição ao servidor
fetch('/equipment-info')
    .then(response => response.json())
    .then(data => {
        // Verifique os dados recebidos da requisição
        //console.log(data);

        // Pré-processamento para contar a quantidade de IDs por cidade
        const countedData = data.reduce((accumulator, item) => {
            const cityName = item.Cidade;

            // Se a cidade ainda não está no acumulador, crie uma entrada para ela
            if (!accumulator[cityName]) {
                accumulator[cityName] = {
                    cityName: cityName,
                    idCount: 0,
                };
            }

            // Incrementa a contagem de IDs para a cidade
            accumulator[cityName].idCount++;

            return accumulator;
        }, {});

        // Extraia os rótulos e valores do pré-processamento
        const labels = Object.keys(countedData);
        const values = Object.values(countedData).map(item => item.idCount);

        // Configuração do gráfico de barras
        const barChartData = {
            labels: labels,
            datasets: [{
                label: 'Quantidade de Equipamentos',
                backgroundColor: 'rgba(75, 192, 192, 0.2)',
                borderColor: 'rgba(75, 192, 192, 1)',
                borderWidth: 1,
                data: values,
            }],
        };

        // Configuração do gráfico de barras
        const barChartOptions = {
            scales: {
                y: {
                    beginAtZero: true,
                },
            },
        };

        // Obtenha o contexto do canvas
        const barChartCanvas = document.getElementById('barChart').getContext('2d');

        // Crie o gráfico de barras
        const barChart = new Chart(barChartCanvas, {
            type: 'bar',
            data: barChartData,
            options: barChartOptions,
        });
    })
    .catch(error => console.error('Erro ao obter dados do servidor:', error));
