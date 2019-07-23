<?php
session_start(); 
require "secury.php";
include('conexao.php');

// Solicita uma query com os dados já modificando a base de tempo para unix timestamp em segundos
$query = 'SELECT hum, temp, volt, pot, UNIX_TIMESTAMP(date_time) AS datetime FROM gprsArduinoTest';

// Executa uma query com dois argumentos e armazena os resultados em $result
$result = mysqli_query($conexao , $query);

// Define uma variável para armazenar as linhas em formato de matriz
$rows = array();

// Define uma variável para armazenar as colunas em formato de matriz
$table = array();

// Define os labels e cria um link com as variáveis para indicar no gráfico
$table['cols'] = array( 
	array( 'label' => 'Date Time',   'type' => 'datetime' ), 
	array( 'label' => 'Humidade (%)', 'type' => 'number'),
	array( 'label' => 'Temperatura (C)', 'type' => 'number'),
	array( 'label' => 'Tensao(V)', 'type' => 'number'),
	array( 'label' => 'Potencia (RSSI)', 'type' => 'number')
);

// Esta função procura e converte o resultado da query anterior, na variável $result, e armazena na variável row
while($row = mysqli_fetch_array($result))
{	
	// Utilizamos algumas palavras chaves "v" e "c"
	$sub_array = array();						// Armazena os dados temporários
	$datetime = explode(".", $row["datetime"]);			// Função Timestamp retorna segundos com tres zeros apos o ponto. Por isso usar explode
	$sub_array[] = array( "v" => 'Date(' . $datetime[0] . '000)' ); // Os tres zeros restantes são para converter os valores em millissegundos
	$sub_array[] = array( "v" => $row["hum"] );			// Armazenamos o valor da umidade juntamente com o dia e hora na variável $sub_array[]
	$sub_array[] = array( "v" => $row["temp"] );			// Armazenamos o valor da temperatura juntamente com o dia e hora na variável $sub_array[]
	$sub_array[] = array( "v" => $row["volt"] );			// Armazenamos o valor da tensão juntamente com o dia e hora na variável $sub_array[]
	$sub_array[] = array( "v" => $row["pot"] );			// Armazenamos o valor da potencia juntamente com o dia e hora na variável $sub_array[]
	$rows[] =  array("c" => $sub_array );				// Gera uma linha e duas células com valores das variá. anteriores junta. com a hora e data
}

$table['rows'] = $rows;			// Armazena todos os resultados, com variáveis, data e hora na variável $table
$jsonTable = json_encode($table);	// Converte todos os dados no formato json, necessário para a API do google chart

?>



<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<meta name="description" content="">
	<meta name="author" content="Eduardo Avelar - EasyTrom Labs">
	<meta name="generator" content="Jekyll v3.8.5">
	<title> Tela Monitoramento &copy;</title>
	
	<link rel="shortcut icon" type="image/png" href="Favicon.png"/>

	<!-- Bootstrap core CSS -->
	<!--link href="bootstrap.min.css" rel="stylesheet"-->
	<link href="sticky-footer.css" rel="stylesheet">
	
	<!-- Carrega API do Bootstrap -->
	<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css">
	
	<!-- Carrega API do AJAX -->		
	<script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
	

	<script type="text/javascript">
		// Carrega o modo de visualização da API e o pacote de gráficos
		google.charts.load('current', {'packages':['corechart']});
		
		// Configura a função callback para mostrar o gráfico quando finalizar o carregamento dos dados
		google.charts.setOnLoadCallback(drawChart);
		
		// Função que cria e popula o grafico com os dados no formato json 
		function drawChart()
		{
			// Cria a tabela de dados no formato Json
			var data = new google.visualization.DataTable(<?php echo $jsonTable; ?>);
			
			// Configura algumas opçoes para gerar o grafico
			var options = {
				title:'Leitura de temperatura, humidade, tensao da bateria e potencia do sinal',
				backgroundColor: '#F2F2F2',			// Configura o background do gráfico
				colors: ['blue','red','black', 'green'],	// configura a cor de cada linha do gráfico
				fontSize: 12,					// Configura o tamanho da letra
				legend:{position:'bottom'},			// Mostra a legenda em baixo
				chartArea:{width:'90%', height:'75%'}		// Configura a área do gráfico
			};
			
			// Instancia e desenha o grafico de linhas, passando alguns parametros, de acordo com a div "line_chart"
			var chart = new google.visualization.LineChart(document.getElementById('line_chart'));
			
			// Instancia e Desenha o grafico com os dados gerados em json e as opçoes configuradas anteriormente
			chart.draw(data, options);
		}
		// Realiza atualização automática da página a cada 2 minutos
		setInterval(function(){ location.reload(); }, 120000);
	</script>

	<style>
		.page-wrapper
		{
			width:1000px;
			margin:0 auto;
		}
		
	</style>
	
</head>

<body>
	<!--Div que cria o cabeçalho -->
	<div class="jumbotron text-center" style="margin-bottom:0">
		<h1>Monitoramento Remoto</h1>
		<p class="text-body font-weight-bold">Seja bem vindo <?php  echo $_SESSION['nameuser'];?></p>

	</div>

	<nav class="navbar navbar-expand-sm bg-dark navbar-dark">

		<a class="btn btn-primary" href="closed.php" role="button">Sair</a>

		<button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#collapsibleNavbar">

			<span class="navbar-toggler-icon"></span>
		</button>

	</nav>

	<div class="page-wrapper">
		<br />
		<h2 align="center">Grafico do perfil de temperatura e Humidade</h2>
		<div id="line_chart" style="width: 100%; height: 400px"></div>
	</div>
	
	
</body>
</html>