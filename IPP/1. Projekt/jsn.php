<?php

error_reporting(E_ERROR | E_PARSE);

mb_regex_encoding('UTF-8');
mb_internal_encoding('UTF-8');

//Globalni promenne
$inputFile = "php://stdin";
$outputFile = "php://stdout";
$argH = '-';
$argN = false;
$argR = false;
$arrayName = "array";
$itemName = "item";
$argS = false;
$argI = false;
$argL = false;
$argC = false;
$arraySize = false;
$indexItems = false;
$start = 1;
$types = false;

parseArguments($argv, $argc);

$input = fopen($inputFile, "r");
$output = fopen($outputFile, "w");
    
if(!$input)
    error("Nelze otevrit vstupni soubor!\n", 2);
    
if(!$output)
    error("Nelze otevrit nebo vytvorit vystupni soubor!\n", 3);
    
fclose($input);
fclose($output);

$file = file_get_contents($inputFile);
$json = json_decode($file, true);

if($json === NULL)
    error("Vstupni soubor je ve spatnem formatu!\n", 2);
    
$writer = new XMLWriter();
$writer->openUri($outputFile);
$writer->setIndent(4);
    
if(!$argN)
    $writer->startDocument('1.0', 'UTF-8');
    
if($argR)
    $writer->startElement($argR);

parseFile($json);

if($argR)
    $writer->endElement();

exit(0);
    
function parseArguments($argv, $argc) //Funkce pro zpracovani argumentu
{        
    /*
        Funkce, ktera zpracuje paramatery prikazove radky. Funkce priradi patricne
        hodnoty do globalnich promennych. V pripade spatne zadanaych parametru funkce zavola
        funkci error(), ktera ukonci skript s patricnym navratovim kodem.
    */
    global $inputFile, $outputFile, $argH, $argN, $argR, $arrayName, $itemName;
    global $argS, $argI, $argL, $argC, $arraySize, $indexItems, $start, $types;
    
    //Kratke argumenty
    $shortopts = "";
    $shortopts .= "h:";
    $shortopts .= "n";
    $shortopts .= "r:";
    $shortopts .= "s";
    $shortopts .= "i";
    $shortopts .= "l";
    $shortopts .= "c";
    $shortopts .= "a";
    $shortopts .= "t";
        
    //Dlouhe argumenty
    $longopts = array
    (
        "help",
        "input:",
        "output:",
        "array-name:",
        "item-name:",
        "start:",
        "types",
        "array-size",
        "index-items",
    );    
    
    $opts = getopt($shortopts, $longopts);
    
    if(!$opts)
    {
        error("Spatne parametry!\n", 1);
    }
    
    if(sizeof($opts) + 1 != $argc)
    {
        error("Spatne parametry!\n", 1);
    }
    
    if(isset($opts['a']) && isset($opts['array-size'])) //Jedna se o stejny parametr
    {
        error("Spatne parametry!\n", 1);
    }
    
    if(isset($opts['t']) && isset($opts['index-items'])) //Jedna se o stejny parametr
    {
        error("Spatne parametry!\n", 1);
    }
    
    if(isset($opts['start']) && !(isset($opts['t']) || isset($opts['index-items'])))
    {
        error("Prepinac start vyzaduje prepinac index-items!\n", 1);
    }
    
    foreach ($opts as $arg => $value) //Cyklus, ve kterem projedeme vsechny zadane parametry
    {            
        switch($arg)
        {
            case "help":
                
                if ($argc === 2) //Parametr help musi byt zadan samostatne
                    printHelp();
                
                else  //Jinak se jedna o chybu
                    error("Spatne parametry!\n", 1);
                
                break;
                
            case "input":
                $inputFile = $value;
                
                if(!file_exists($inputFile))
                    error("Vstupni soubor neexistuje!\n", 2);
                
                break;
                
            case "output":
                $outputFile = $value;
                break;
            
            case "h":
                $argH = $value;
                break;
            
            case "n":
                $argN = true;
                break;
            
            case "r":
                $argR = $value;
                break;
            
            case "array-name":
                $arrayName = $value;
                
                if(problematicChar($arrayName))
                    error("Chybne zadano jmeno pole!\n", 50);
                
                break;
            
            case "item-name":
                $itemName = $value;
                
                if(problematicChar($itemName))
                    error("Chybne zadano jmeno polozky!", 50);
                
                break;
            case "s":
                $argS = true;
                break;
            
            case "i":
                $argI = true;
                break;
            
            case "l":
                $argL = true;
                break;
            
            case "c":
                $argC = true;
                break;
            
            case "a":
                $arraySize = true;
                break;
            
            case "array-size":
                $arraySize = true;
                break;
            
            case "t":
                $indexItems = true;
                break;
            
            case "index-items":
                $indexItems = true;
                break;
            
            case "start":
                $start = $value;
                
                if(!(is_numeric($start)) || $start < 0)
                    error("Spatne zadane argumenty!\n", 1);
                
                break;
            case "types":
                $types = true;
                break;
        }
    }
}
    
function parseFile($json)
{
    /*
        Hlavni funkce programu. Program postupne projizdi vstupni JSON soubor
        a prevadi ho do formatu XML. V pripade, ze hodnota nejakeho objektu je pole
        je funkce zavolana rekurzivne.
    */
    
    global $argS, $argI, $argL, $argC, $arraySize, $indexItems, $start, $types;
    global $arrayName, $itemName, $writer;
    $first = true;
    $array = false;
    $end = false;
    $change = false;
    $i = 0;
    $text = "";
    
    foreach($json as $item => $value) //Cyklus, ve kterem se projizdi vsechny objekty
    {                
        if(gettype($item) === "integer" && $first)
        {
            $array = true;
            $writer->startElement($arrayName);
            $first = false;
            
            if($arraySize)
                $writer->writeAttribute ("size", count($json));
        }
        
        if(!$argC && !$argS)
        {
            if(gettype($value) === "string" && problematicChar($value))
                $change = true;
        }
        
        if(!$array)
        {
            if(problematicChar($item))
                $item = replaceChar($item);
            
            if(!$change)
            {
                $return = $writer->startElement($item);
            
                if(!$return)
                    error("Chybny element!\n", 51);
            }
        }
        
        else if($array && !$change)
        {
            $writer->startElement($itemName);
            
            if($indexItems)
            {
                $writer->writeAttribute('index', $start);
                $start++;
            }
        }
        
        if(gettype($value) === "boolean")
        {
            if($value)
            {
                if(!$argL)
                    $writer->writeAttribute('value', 'true');
                
                else
                    $writer->writeElement ("true");
            }
               
            else
            {
                if(!$argL)
                    $writer->writeAttribute('value', 'false');
                
                else
                    $writer->writeElement ("false");
            }
        }
            
        else if(gettype($value) === "NULL")
        {
            if(!$argL)
                $writer->writeAttribute('value', 'null');
            
            else
                $writer->writeElement("null");
        }
        
        else if(gettype($value) === "string")
        {
            if(!$argS)
            {
                if($change)
                {                                       
                    if($array && $indexItems)
                    {
                        $text = rawAtrrField($value, $start);
                        $start++;
                    }
                    
                    else if($array)
                        $text = rawAttr($value, $itemName);
                    
                    else
                        $text = rawAttr($value, $item);
                    
                    $writer->writeRaw($text);
                }
                
                else
                {                   
                    //Mozna jeste ten element
                    $writer->writeAttribute('value', $value);
                }
            }
            
            else
            {
                if($argC)
                    $writer->text($value);
                
                else
                {
                    $text = rawText($value);
                    $writer->writeRaw($text);
                }
            }
        }
        
        else if(gettype($value) === "integer")
        {
            if(!$argI)
                $writer->writeAttribute('value', $value);
            
            else
                $writer->text($value);
        }
        
        else if(gettype($value) === "double")
        {
            if(!$argI)
                $writer->writeAttribute ('value', floor($value));
            
            else
                $writer->text(floor($value));
        }
        
        if($types)
        {
            $type = myGetType($value);
            $writer->writeAttribute('type', $type);
        }
        
        if(is_array($value))
            parseFile($value);
        
        if(!$change)
            $writer->endElement();
        
        if(($i > 0) && $array && (gettype($item) != "integer" || $item === 0))
        {
            $first = true;
            $array = false;
            $writer->endElement();
            $i = 0;
            $end = true;
        }
        
        if($array)
            $i++;
    }
    
    if(!$end && $array)
        $writer->endElement();
}

function myGetType($value)
{
    /*
        Funkce, ktera zjisti datovy typ hodnoty v JSON. Funkce vraci dany typ
        podle specifikace zadani.
    */
    
    $item = gettype($value);
    
    switch($item)
    {
        case "integer":
            return $item;
        
        case "double":
            return "real";
        
        case "string":
            return $item;
            
        default:
            return "literal";
    }
}

function problematicChar($value)
{
    /*
        Funkce, ktera zjisti, jestli retezec obsahuje problematicke XML
        znaky. V pripade, ze obsahuje funkce vraci hodnotu true, v opacnem
        pripade vraci hodnotu false.
    */
    
    $pattern = "[<>&'";
    $pattern .= '"]';
    
    $result = mb_ereg($pattern, $value);
    
    return $result;
}

function replaceChar($value)
{
    /*
        Funkce, ktera v retezci nahrazuje problematicke znaky. Funkce vraci
        retezec, kde jsou problematicke znaky nahrazeny pozadovanym znakem.
    */
    
    global $argH;
    
    $pattern = "[<>&/'";
    $pattern .= '"]';
    
    $result = mb_ereg_replace($pattern, $argH, $value);
    
    return $result;
}

function rawAttr($value, $item)
{
    /*
        Funkce pro zapis hondot s problematickymi znaky do atributu. 
    */
    
    $string = "\n<";
    $string .= "$item";
    $string .= " value=";
    $string .= '"';
    $string .= "$value";
    $string .= '"';
    $string .= "/>\n";
    
    return $string;
}

function rawText($value)
{
    /*
        Funkce pro zapis hondot s problematickymi znaky do textoveho elementu.
    */
    
    $string = "\n";
    $string .= "$value\n";
    
    return $string;
}

function rawAtrrField($value, $start)
{
    /*
        Funkce pro zapis hondot s problematickymi znaky do pole.
    */    
    
    global $itemName;
    
    $string = "\n<";
    $string .= "$itemName";
    $string .= " index=";
    $string .= '"';
    $string .= "$start";
    $string .= '"';
    $string .= " value=";
    $string .= '"';
    $string .= "$value";
    $string .= '"';
    $string .= "/>\n";
}

function printHelp() //Vytiskne napovedu
{
    echo "Skript pro prevod JSON fromatu do XML\n";
    echo "Pouziti skriptu: \n";
    echo "--help\t\t\t\tVyvola tuto napovedu\n";
    echo "--input=filename\t\tVstupni soubor\n";
    echo "--output=filename\t\tVystupni soubor\n";
    echo "-h=subst\t\t\tNepovoleny znak se nahradi retezcem\n";
    echo "-n\t\t\t\tNegenrovat XML hlavicku\n";
    echo "-r=root-element\t\t\tJmeno paroveho korenoveho elementu obalujici vysledek\n";
    echo "--array-name=array-element\tPrejemnovani elementu obsahujici pole\n";
    echo "--item-name=item-element\tZmena jmena elementu pro pprvky pole\n";
    echo "-s\t\t\t\tString transofrmovan na textovy element\n";
    echo "-i\t\t\t\tNumber transofrmovan na textovy element\n";
    echo "-l\t\t\t\tLiteraly transformovany na elementy\n";
    echo "-c\t\t\t\tPreklad probelmatickych znaku\n";
    echo "-a, --aray-size\t\t\tU pole doplnen atribut size\n";
    echo "-t, --index-items\t\tKe kazdemu prvku pole pridan atribut index\n";
    echo "--start=n\t\t\tInicializace inkremetacniho citace\n";
    echo "--types\t\t\t\tSkalarni hodnota doplnena o atribut type\n";
    exit(0);
}

function error($err, $errorCode) //Vypis chyby a ukonceni skriptu
{
    fwrite(STDERR, $err);
    exit($errorCode);
}