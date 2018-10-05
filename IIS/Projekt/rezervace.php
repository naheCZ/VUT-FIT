<?php
  include "header.html";
  session_start();
  require_once('Db.php');
  Db::connect('localhost', 'xnahal01', 'xnahal01', 'om3gemsa');
  
  if(!isset($_SESSION['user_id']))
  {
    header('Location: index.php');
    exit();
  }
  
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
        <li class="nav-item">
          <a class="nav-link" href="info.php">O nás</a>
        </li>
        <li class="nav-item">
          <a class="nav-link" href="jidelak.php">Nabídka pokrmů</a>
        </li>
        <li class="nav-item dropdown active">
            <a class="nav-link dropdown-toggle" href="#" id="rezervace" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Rezervace</a>
            <div class="dropdown-menu" aria-labelledby="rezervace">
              <a class="dropdown-item active" href="rezervace.php">Moje rezervace</a>
              <a class="dropdown-item" href="nova.php">Vytvořit novou rezervaci</a>
            </div>
        </li>
        <?php
          if($_SESSION['privileges'] == 2)
          {
            echo('<li class="nav-item dropdown">
                <a class="nav-link dropdown-toggle" href="#" id="objednavky" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Objednávky</a>
                <div class="dropdown-menu" aria-labelledby="objednavky">
                  <a class="dropdown-item" href="objednavka.php">Vytvořit novou objednávku</a>
                </div>
            </li>');
          }
        ?>
      </ul>
      <ul class="nav navbar-nav navbar-right">
        <li><a href="rezervace.php?odhlasit"><span class="fa fa-sign-out"></span> Odhlásit</a></li>
      </ul>
    </div>
  </nav>

  <div class="container-fluid text-center">
    <div class="row content">
      <div class="col-sm-2 sidenav">
      </div>
      <div class="col-sm-8 text-left">
        <h1>Seznam rezervací</h1>
        <table class="table table-hover">
          <thead>
            <th>ID</th>
            <th>Datum</th>
            <th>Název</th>
            <th>Počet osob</th>
          </thead>
          <tbody>
            <?php
              
              if($_SESSION['privileges'] == 1)
              {
                $rezervace = Db::queryAll
                ('
                  SELECT *
                  FROM rezervace
                  WHERE user_id=? 
                ', $_SESSION['user_id']);
              }
              
              else
              {
                $rezervace = Db::queryAll
                ('
                  SELECT *
                  FROM rezervace 
                ');
              }
              
              foreach($rezervace as $rez)
              {
                $rezID = htmlspecialchars($rez['id_rezervace']);
                $rezDatum = htmlspecialchars($rez['datum']);
                $rezNazev = htmlspecialchars($rez['nazev']);
                $rezPocet = htmlspecialchars($rez['pocet_osob']);
                
                echo('<tr><td>' . $rezID);
                echo('</td><td>' . $rezDatum);
                echo('</td><td>' .$rezNazev);
                echo('</td><td>' .$rezPocet);
                echo('</td></tr>');
              }
              ?>  
          </tbody>
        </table>
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
