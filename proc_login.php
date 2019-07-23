<?php

session_start();

require 'conexao.php';
$email = $_POST['emailc'];
$senha = $_POST['passwordc'];

$descrypt = md5($senha);

$query = "SELECT * FROM usuario WHERE email='$email' AND senha='$descrypt' LIMIT 1";

$exec = $conexao->query($query);

$result = $exec->fetch_assoc();

if (empty($result)) {
	$_SESSION['errorlogin'] = "Erro: Usuario ou Senha Invalidos";
	header("Location: index.php");
	exit();
} else {
	$_SESSION['iduser'] = $result['id'];
	$_SESSION['nameuser'] = $result['nome'];
	header("Location: logado.php");
	exit();
}

?>