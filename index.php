<?php 
session_start(); 
?>

<!doctype html>
<html lang="pt-br">
<head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE-edge">
  <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

  <meta name="description" content="">
  <meta name="author" content="Eduardo Avelar - EasyTrom Labs - Based on Bootstrap">
  <meta name="generator" content="Jekyll v3.8.5">
  <link rel="icon" type="image/png" href="Favicon.png"/>>
  <title>Sistema de acesso - EasyTrom Labs</title>

  <!-- Bootstrap core CSS -->
  <link href="bootstrap.min.css" rel="stylesheet">


  <style>
    .bd-placeholder-img {
      font-size: 1.125rem;
      text-anchor: middle;
      -webkit-user-select: none;
      -moz-user-select: none;
      -ms-user-select: none;
      user-select: none;
    }

    @media (min-width: 768px) {
      .bd-placeholder-img-lg {
        font-size: 3.5rem;
      }
    }
  </style>
  <!-- Estilo curstomizado para este template -->
  <link href="signin.css" rel="stylesheet">
</head>

<?php  
// Destroi as variáveis especificadas
unset($_SESSION['iduser'], $_SESSION['nameuser']);

?>

<body class="text-center">

  <div class="container">

    <form class="form-signin" method="post" action="proc_login.php">

      <img class="mb-4" src="Favicon.png" alt="" width="100" height="100">
      <h1 class="h3 mb-3 font-weight-normal">Login de usuarios</h1>
      <label for="inputEmail" class="sr-only">Endereço de email</label>
      <input type="email" id="inputEmail" class="form-control" placeholder="Endereço de email" name="emailc" required autofocus><br>
      <label for="inputPassword" class="sr-only">Senha</label>
      <input type="password" id="inputPassword" class="form-control" placeholder="Senha" name="passwordc" required><br>

      <button class="btn btn-lg btn-primary btn-block" type="submit">Entrar</button>
    </form>

    <p class="text-center text-danger">
      <?php 
      if (isset($_SESSION['errorlogin'])) {
        echo $_SESSION['errorlogin'];
        unset($_SESSION['errorlogin']);        
      }
      ?>
    </p>
    <p class="text-center text-danger">
      <?php  
      if (isset($_SESSION['secury'])) {
        echo $_SESSION['secury'];
        unset($_SESSION['secury']);
      }
      ?>     
    </p>

    <p class="mt-5 mb-3 text-muted" >&copy; <a href="https://easytromlabs.com/" target="_blank">EasyTrom Labs</a></p>
  </div>
</body>
</html>
