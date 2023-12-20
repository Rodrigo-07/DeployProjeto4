// Faz a requisição ao servidor
fetch('/equipment-info')
    .then(response => response.json())
    .then(data => {
        // Verifique os dados recebidos da requisição
        //console.log(data);

        // Contagem de ativos conectados e desconectados
        const connectedCount = data.filter(item => item.State === 'Conectado').length;
        const disconnectedCount = data.filter(item => item.State === 'Desconectado').length;

        // Dados para o gráfico de pizza
        const pieChartData = {
            labels: ['Desconectados', 'Conectados'], // Ordem invertida aqui
            datasets: [{
                data: [connectedCount, disconnectedCount], // Ordem invertida aqui
                backgroundColor: ['red', 'green'],
            }],
        };

        // Configuração do gráfico de pizza
        const pieChartOptions = {
            responsive: true,
            maintainAspectRatio: false,
        };

        // Obtenha o contexto do canvas
        const pieChartCanvas = document.getElementById('pieChart').getContext('2d');

        // Crie o gráfico de pizza
        const pieChart = new Chart(pieChartCanvas, {
            type: 'doughnut',
            data: pieChartData,
            options: pieChartOptions,
        });
    })
    .catch(error => console.error('Erro ao obter dados do servidor:', error));
