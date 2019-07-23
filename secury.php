<?php

ob_start();

if ($_SESSION['iduser'] =="" || $_SESSION['nameuser']=="") {
	$_SESSION['secury'] = "Error faça login";
	header("Location: index.php");
	exit();
}

?>