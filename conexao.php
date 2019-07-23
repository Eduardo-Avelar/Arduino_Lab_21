<?php
// Estabelece a conexão com o banco de dados
require 'config.php';
//$conexao = mysqli_connect(HOST, USUARIO, SENHA, DB) or die('Não foi possivel conectar ao banco de dados.');

$conexao = new mysqli(HOST, USUARIO, SENHA, DB);

if ($conexao->connect_error) {
	echo "Erro ao conectar ao banco de dados". $conexao->connect_error;
}else {
	//echo "Conexao com o banco de dados feita com sucesso";
}
?>