                                            <? header("Content-Type: text/html; charset=UTF-8");?>
<?php
  require_once('Db.php');
  Db::connect('localhost', 'xnahal01', 'xnahal01', 'om3gemsa');
  session_start();
  if(!isset($_SESSION['uzivatel_id']))
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
  
  if($_POST)
  {
    if($_POST['operace'] == 'in')
    {
      if($_POST['prihl'] < $_POST['kap'])
      {
        Db::query
        ('
        INSERT INTO reference (uzivatel, konzultace)
        VALUES(?, ?)
        ', $_POST['uzi_id'], $_POST['kon_id']);
      
        Db::query
        ('
          UPDATE konzultace
          SET prihlaseno=prihlaseno + 1
          WHERE kon_id=?
          ', $_POST['kon_id']);
      }
      
      else
        $zprava = 'Konzultace je jiz obsazena, nelze se prihlasit!';
      
    }
    
    else if($_POST['operace'] == 'out')
    { 
      Db::query
      ('
        DELETE FROM reference
        WHERE ref_id=?
      ', $_POST['id']);
      
      Db::query
      ('
        UPDATE konzultace
        SET prihlaseno=prihlaseno - 1
        WHERE kon_id=?
      ', $_POST['kon_id']);
    }
    
    else if($_POST['operace'] == 'delete')
    {
       Db::query
      ('
        DELETE FROM konzultace
        WHERE kon_id=?
      ', $_POST['kon_id']);
    }
  }
?>

<!DOCTYPE html>
<html lang="cs-cz">
  <head>
    <meta charset="utf-8" />
    <title>Konzultační systém</title>
    <link rel="stylesheet" type="text/css" href="styly.css">
    <script type="text/javascript" src="skripty.js"></script>
  </head>
  <body>
    <header>
      <div id="logo"><h1>Správa konzultačních schůzek</h1></div>
      <nav>
        <ul>
          <a href="index.php"><li>Přihlášení</li></a>
          <a href="zobrazit.php"><li>Moje konzultace</li></a>
          <a href="prochazet.php"><li class="aktivni">Dostupné konzultace</li></a>
          <a href="vytvareni.php"><li>Vytvořit konzultaci</li></a>
        </ul>
      </nav>
      <a href="prochazet.php?odhlasit"><div id="off"><h1>Odhlásit se</h1></div></a>
    </header>
    <article>
      <div id = "centrovac">
      <section>
        <h2>Dostupné konzultace</h2>
        <?php
          if(isset($zprava))
            echo('<p>' . $zprava . '</p>');
          $kon = Db::queryAll
          ('
            SELECT *
            FROM konzultace
          ');
          
          $zap = Db::queryAll
          ('
            SELECT *
            FROM reference
          ');
          
          echo('<table>');
          echo('<tr>');
          echo('<tr><th>Předmět</th>');
          echo('<th>Učitel</th>');
          echo('<th>Datum</th>');
          echo('<th>Začátek</th>');
          echo('<th>Konec</th>');
          echo('<th>Kapacita</th>');
          echo('<th>Přihlášeno</th>');
          echo('<th>Akce</th>');
          echo('</tr>');
          $u_id = htmlspecialchars($_SESSION['uzivatel_id']);
          $u_ty = htmlspecialchars($_SESSION['uzivatel_ucitel']);
          $b = 0;
          $d = 0;
          foreach ($kon as $k)
          {
            $k_id = htmlspecialchars($k['kon_id']);
            $k_pr = htmlspecialchars($k['prihlaseno']);
            $k_ka = htmlspecialchars($k['kapacita']);
            if($k['vytvoril'] == $u_id)
              $d = 1;
            else
              $d = 0;
            echo('<form method="post">');
            echo('<tr><td>' . htmlspecialchars($k['predmet']));
            echo('</td><td>' . htmlspecialchars($k['ucitel']));
            echo('</td><td>' . htmlspecialchars($k['datum']));
            echo('</td><td>' . htmlspecialchars($k['zacatek']));
            echo('</td><td>' . htmlspecialchars($k['konec']));
            echo('</td><td>' . htmlspecialchars($k['kapacita']));
            echo('</td><td>' . htmlspecialchars($k['prihlaseno']));
            foreach ($zap as $z)
            {
              if($z['uzivatel'] == $u_id && $z['konzultace'] == $k_id)
              {
                $b = 1;
                $c = $z['ref_id'];
              }
                
              else
              {
                $b = 0;
                $c = $z['ref_id'];
              }             
            }
            if($b && !$u_ty)
            {
              echo('</td><td> <input type="submit" value="Odhlásit" />');
              echo('<input type="hidden" name="operace" value="out" />');
              echo('<input type="hidden" name="id" value="'. $c .'" />');
              echo('<input type="hidden" name="kon_id" value="'. $k_id .'" />');
            }
            
            else if(!$b && !$u_ty)
            {
              echo('</td><td> <input type="submit" value="Přihlásit" />');
              echo('<input type="hidden" name="operace" value="in" />');
              echo('<input type="hidden" name="kon_id" value="'. $k_id .'" />');
              echo('<input type="hidden" name="uzi_id" value="'. $u_id .'" />');
              echo('<input type="hidden" name="prihl" value="'. $k_pr .'" />');
              echo('<input type="hidden" name="kap" value="'. $k_ka .'" />');
            }
            
            else if($d && $u_ty)
            {
              echo('</td><td> <input type="submit" value="Odstranit" />');
              echo('<input type="hidden" name="operace" value="delete" />');
              echo('<input type="hidden" name="kon_id" value="'. $k_id .'" />');
              echo('</td><td><a href="vytvareni.php?upravit=' . $k_id .'"><button type="button">Upravit</button></a>');
              echo('</td><td><a href="prihlaseni.php?id=' . $k_id .'"><button type="button">Přihlášeni</button></a>');  
            }
            echo('</form>');
            echo('</td></tr>');
          }
          echo('</table>');
        ?>
        <br /><br />
      </section>
      <div class="cistic"></div>
      </div>
      </article>
      <footer>
      <p>Vytvořil Roman Nahálka v rámci projektu ITU 2016/2017 pro FIT VUT Brno</p>
    </footer>
    </body>
</html>