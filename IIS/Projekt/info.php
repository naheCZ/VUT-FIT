<?php
  include "header.html";
  require_once('Db.php');
  Db::connect('localhost', 'xnahal01', 'xnahal01', 'om3gemsa');
  session_start();
  
  if(isset($_SESSION['user_id']))
    $prihlasen = true;
    
  if(isset($_GET['odhlasit']))
  {
    session_destroy();
    header('Location: index.php');
    exit();
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
        <li class="nav-item active">
          <a class="nav-link" href="#">O nás <span class="sr-only">(current)</span></a>
        </li>
        <li class="nav-item">
          <a class="nav-link" href="jidelak.php">Nabídka pokrmů</a>
        </li>
        <?php
          if(isset($prihlasen))
          {
            echo('<li class="nav-item dropdown">
                <a class="nav-link dropdown-toggle" href="#" id="rezervace" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Rezervace</a>
                <div class="dropdown-menu" aria-labelledby="rezervace">
                  <a class="dropdown-item" href="rezervace.php">Moje rezervace</a>
                  <a class="dropdown-item" href="nova.php">Vytvořit novou rezervaci</a>
                </div>
            </li>');
            if($_SESSION['privileges'] == 2)
            {
              echo('<li class="nav-item dropdown">
                  <a class="nav-link dropdown-toggle" href="#" id="objednavky" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Objednávky</a>
                  <div class="dropdown-menu" aria-labelledby="objednavky">
                    <a class="dropdown-item" href="objednavka.php">Vytvořit novou objednávku</a>
                  </div>
              </li>');
            }
          }
        ?>
      </ul>
      <ul class="nav navbar-nav navbar-right">
        <?php
          if(!isset($prihlasen))
            echo('<li><a href="prihlaseni.php"><span class="fa fa-sign-in"></span> Přihlásit&nbsp</a></li>)');
          else
            echo('<li><a href="info.php?odhlasit"><span class="fa fa-sign-out"></span> Odhlásit</a></li>');
        ?>
      </ul>
    </div>
  </nav>

  <div class="container-fluid text-center">
    <div class="row content">
      <div class="col-sm-2 sidenav">
      </div>
      <div class="col-sm-8 text-left">
        <h1>O nás</h1>
        Dobrý den, vítejte na stránkách naší restaurace s názvem Kachnička u Smrčku.<br>
        Jsme klasická česká restaurace, fungující přímo v areálu FIT VUT.<br>
        Restaurace slouží pouze pro studenty a zaměstance fakulty, tudíž je nutné být u nás zaregistrován.<br>
        Na našich stránkách je možné dělat online objednávky, pro tuto funkci je však nutné se přihlásit do systému.<br>
        Pro přihlášení, jděte prosím na stránku s přihlášením. Na stránku se dostane přes horní menu. 
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
