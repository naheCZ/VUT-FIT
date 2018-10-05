<?php
  include "header.html";
  session_start();
  require('Db.php');
  Db::connect('localhost', 'xnahal01', 'xnahal01', 'om3gemsa');
  
  if ($_POST)
  {
          $uzivatel = Db::queryOne('
                  SELECT user_id, privileges
                  FROM users
                  WHERE username=? AND password=?
          ', $_POST['usr'], $_POST['pass']);
          if (!$uzivatel)
                  $zprava = 'Neplatné uživatelské jméno nebo heslo';
          else
          {
                  $_SESSION['user_id'] = $uzivatel['user_id'];
                  $_SESSION['privileges'] = $uzivatel['privileges'];
                  header('Location: index.php');
                  exit();
          }
  }
?> 

<body>

  <nav class="navbar navbar-expand-md navbar-dark bg-dark fixed-top">
    <a class="navbar-brand" href="#">Menu</a>
    <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarsExampleDefault" aria-controls="navbarsExampleDefault" aria-expanded="false" aria-label="Toggle navigation">
      <span class="navbar-toggler-icon"></span>
    </button>

    <div class="collapse navbar-collapse" id="navbarsExampleDefault">
      <ul class="navbar-nav mr-auto">
        <li class="nav-item">
          <a class="nav-link" href="info.php">O nás</a>
        </li>
        <li class="nav-item">
          <a class="nav-link" href="jidelak.php">Nabídka pokrmů</a>
        </li>
        <li class="nav-item dropdown">
            <a class="nav-link dropdown-toggle" href="#" id="rezervace" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Rezervace</a>
            <div class="dropdown-menu" aria-labelledby="rezervace">
              <a class="dropdown-item" href="rezervace.php">Moje rezervace</a>
              <a class="dropdown-item" href="nova.php">Vytvořit novou rezervaci</a>
            </div>
        </li>
      </ul>
      <ul class="nav navbar-nav navbar-right">
        <li><a href="prihlaseni.php"><span class="fa fa-sign-in"></span> Přihlásit&nbsp</a></li>
      </ul>
    </div>
  </nav>

  <div class="container-fluid text-center">
    <div class="row content">
      <div class="col-sm-2 sidenav">
      </div>
      <div class="col-sm-8 text-left">
        <h1>Přihlášení do systému</h1>
        <?php
          if(isset($zprava))
            echo('<p>' . $zprava . '</p>');
        ?>
        <form method="post">
          <div class="form-group row">
            <label for="usr" class="col-2 col-form-label">Jméno:</label>
            <div class="col-10">
              <input type="text" class="form-control" id="usr" placeholder="Zadejte uživatelské jméno" name="usr">
            </div>
          </div>
          <div class="form-group row">
            <label for="pass" class="col-2 col-form-label">Heslo:</label>
            <div class="col-10">
              <input type="password" class="form-control" id="pass" placeholder="Zadejte heslo" name="pass">
            </div>
          </div>
          <button type="submit" class="btn btn-default">Odeslat</button>
        </form>
      </div>
      <div class="col-sm-2 sidenav">
        <div class="well">
        </div>
      </div>
    </div>
  </div>
    
<?php
include "footer.html"
?> 
