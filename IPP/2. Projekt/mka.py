#!/usr/bin/env python3

import argparse
import sys
from enum import Enum

class States(Enum):
    start = 0,
    finiteSetStart = 1,
    finiteSet = 2,
    finiteSetEnd = 3,
    inputAlphabetStart = 4,
    inputAlphabet1 = 5,
    inputAlphabet2 = 6,
    inputAlphabet3 = 7,
    inputAlphabet4 = 8,
    inputAlphabetEnd = 9,
    rulesStart = 10,
    rules1 = 11,
    rules2 = 12,
    rules3 = 13,
    rules4 = 14,
    rules5 = 15,
    rules6 = 16,
    rules7 = 17,
    rules8 = 18,
    rulesEnd = 19,
    startStateStart = 20,
    startStateEnd = 21,
    endStateStart = 22,
    endState = 23,
    endStateEnd = 24,
    end = 25

def main():
    args = parseArguments()

    if args.input:
        try:
            fileInput = open(args.input, 'r')

        except:
            error('Zadany vstupni soubor neexistuje!', 2)

    else:
        fileInput = sys.stdin

    if args.output:
        try:
            fileOutput = open(args.output, 'w')

        except:
            error('Nelze otevrit nebo vytvorit vystupni soubor!', 3)

    else:
        fileOutput = sys.stdout

    automata = parseFile(fileInput, args)

    isWellSpecified(automata)

    if args.m:
        automata = minimize(automata)
        normalizedOutput(fileOutput, automata)

    elif args.f:
        findNonFinishing(automata, fileOutput)

    else:
        normalizedOutput(fileOutput, automata)

    fileOutput.close()
    fileInput.close()
    sys.exit(0)

def getNextToken(file, skip):
    char = file.read(1)
    if char == '#':
        file.readline()
        return getNextToken(file, skip)

    if skip:
        if char == '\n':
            return getNextToken(file, skip)

        if char == ' ':
            return getNextToken(file, skip)

        if char == '\t':
            return  getNextToken(file, skip)

    return char

def parseFile(file, args):
    char = 'start'
    symbol = ''
    oneRule = []
    fileState = States.start
    states = []
    alphabet = []
    rules = []
    startState = ''
    endStates = []
    automata = []
    skip = True
    i = 0

    while(1):
        char = getNextToken(file, skip)
        i  += 1
        skip = True

        if args.i:
            char = char.lower()

        if char == '':
            if i == 1:
                error('Prazdny vstup!', 60)
            break

        if fileState == States.start:
            if char != '(':
                error('Lexikalni chyba1!', 60)
            else:
                fileState = States.finiteSetStart
                continue

        if fileState == States.finiteSetStart:
            if char != '{':
                error('Lexikalni chyba2!', 60)
            else:
                fileState = States.finiteSet
                continue

        if fileState == States.finiteSet:
            if char != ',' and char != '}':
                symbol += char
                continue
            elif char == ',':
                if symbol not in states:
                    states.append(symbol)
                symbol = ''
                continue
            elif char == '}':
                fileState = States.finiteSetEnd
                if symbol not in states:
                    states.append(symbol)
                symbol = ''
                continue

        if fileState == States.finiteSetEnd:
            if char != ',':
                error('Lexikalni chyba3!', 60)
            else:
                fileState = States.inputAlphabetStart
                continue

        if fileState == States.inputAlphabetStart:
            if char != '{':
                error('Lexikalni chyba4!', 60)
            else:
                fileState = States.inputAlphabet1
                continue

        if fileState == States.inputAlphabet1:
            if char != "'":
                if char == '}' and not alphabet:
                    error('Prazdna vstupni abeceda!', 61)

                error('Lexikalni chyba5!', 60)
            else:
                fileState = States.inputAlphabet2
                skip = False
                continue

        if fileState == States.inputAlphabet2:
            if char == "'":
                if 'empty' not in alphabet:
                    alphabet.append('empty')
                fileState = States.inputAlphabet4
                continue
            else:
                if char not in alphabet:
                    alphabet.append(char)
                fileState = States.inputAlphabet3
                skip = False
                continue

        if fileState == States.inputAlphabet3:
            if char != "'":
                error('Lexikalni chyba6!', 60)
            else:
                fileState = States.inputAlphabet4
                continue

        if fileState == States.inputAlphabet4:
            if char != ',' and char != '}':
                error('Lexikalni chyba7!', 60)
            elif char == ',':
                fileState = States.inputAlphabet1
                continue
            else:
                fileState = States.inputAlphabetEnd
                continue

        if fileState == States.inputAlphabetEnd:
            if char != ',':
                error('Lexikalni chyba8!', 60)
            else:
                fileState = States.rulesStart
                continue

        if fileState == States.rulesStart:
            if char != '{':
                error('Lexikalni chyba9!', 60)
            else:
                fileState = States.rules1
                continue

        if fileState == States.rules1:
            if char != "'":
                symbol += char
                continue
            else:
                if symbol not in states:
                    error('Spatny symbol  v pravidlech!1', 61)
                oneRule.append(symbol)
                symbol = ''
                fileState = States.rules3
                skip = False
                continue

        if fileState == States.rules3:
            if char == "'":
                if 'empty' not in alphabet:
                    error('Spatny symbol v pravidlech!', 61)
                else:
                    oneRule.append('empty')
                    fileState = States.rules5
                    continue

            if char not in alphabet:
                error('Spatny symbol v pravidlech2!', 61)
            else:
                oneRule.append(char)
                fileState = States.rules4
                continue

        if fileState == States.rules4:
            if char != "'":
                error('Lexikalni chyba10!', 60)
            else:
                fileState = States.rules5
                continue

        if fileState == States.rules5:
            if char != '-':
                error('Lexikalni chyba11!', 60)
            else:
                fileState = States.rules6
                skip = False
                continue

        if fileState == States.rules6:
            if char != '>':
                error('Lexiklani chyba12!', 60)
            else:
                fileState = States.rules7
                continue

        if fileState == States.rules7:
            if char != ',' and char != '}':
                symbol += char
                continue

            if symbol not in states:
                error('Spatny symbol  v pravidlech!', 61)
            oneRule.append(symbol)
            if oneRule not in rules:
                rules.append(oneRule)
            symbol = ''
            oneRule = []

            if char == ',':
                fileState = States.rules1
                continue

            elif char == '}':
                fileState = States.rulesEnd
                continue

        if fileState == States.rulesEnd:
            if char != ',':
                error('Lexikalni chyba13!', 60)
            else:
                fileState = States.startStateStart
                continue

        if fileState == States.startStateStart:
            if char != ',':
                symbol += char
                continue
            else:
                if symbol not in states:
                    error('Spatny symbol v pravidlech4!', 61)
                startState = symbol
                symbol = ''
                fileState = States.endStateStart
                continue

        if fileState == States.endStateStart:
            if char != '{':
                error('Lexikalni chyba14!', 60)
            else:
                fileState = States.endState
                continue

        if fileState == States.endState:
            if char != ',' and char != '}':
                symbol += char
                continue

            if symbol not in states:
                if not endStates:
                    error('Prazna mnozina koncovych stavu!', 62)
                error('Spatny symbol c pravidlech!5', 61)
            endStates.append(symbol)
            symbol = ''

            if char == ',':
                continue

            if char == '}':
                fileState = States.end
                continue

        if fileState == States.end:
            if char != ')':
                error('Lexikalni chyba15!', 60)
            fileState = States.start
            continue

    if not alphabet:
        error('Prazdna vstupni abeceda!', 61)

    if compare(states, endStates):
        error('Mnozina koncovych stavu neni podmnozinou stavu!', 61)

    stateControl(states)

    automata.append(states)
    automata.append(alphabet)
    automata.append(rules)
    automata.append(startState)
    automata.append(endStates)

    return automata

def stateControl(states):
    for s in states:
        if s.startswith('_'):
            error('Stav nesmi zacinat podtrzitkem!', 60)
        if s[0].isdigit():
            error('Stav nesmi zacinat cislem!', 60)
        if s.endswith('_'):
            error('Stav nesmi koncit podtrziykem!', 60)


def parseArguments():
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument('--help', action='store_true', help='Vypise napovedu', dest='help')
    parser.add_argument('--input', action='store', help='Vstupni soubor', dest='input')
    parser.add_argument('--output', action='store', help='Vystupni soubor', dest='output')
    parser.add_argument('-f', '--find-non-finishing', action='store_true', help='Hleda neukoncujici stav zadaneho DSKA', dest='f')
    parser.add_argument('-m', '--minimize', action='store_true', help='Provede minimalizaci DSKA', dest='m')
    parser.add_argument('-i', '--case-insensitive', action='store_true', help='Nebude bran ohled na velikosti znaku', dest='i')

    try:
        args = parser.parse_args()
    except:
        error('Zadan spatny argument!', 1)

    params = ['--help', '--input', '--output', '-f', '-m', '-i']

    for arg in params:
        if sys.argv.count(arg) > 1:
            error('Nelze zadat jeden parametr vicekrat!', 1)

    if sys.argv.count('-f') == 1 and sys.argv.count('--find-non-finishing') == 1:
        error('Nelze zadat jeden parametr vicekrat!', 1)

    if sys.argv.count('-m') == 1 and sys.argv.count('--minimize') == 1:
        error('Nelze zadat jeden parametr vicekrat!', 1)

    if sys.argv.count('-i') == 1 and sys.argv.count('--case-insensitive') == 1:
        error('Nelze zadat jeden parametr vicekrat!', 1)

    if args.help:
        if(len(sys.argv) != 2):
            error('Parametr help nelze kombinovat', 1)

        else:
            printHelp(parser)

    if args.f and args.m:
        error('Argumenty m a f nelze kombinovat!', 1)

    return args

def compare(states, endStates):
    states.sort()
    endStates.sort()

    if states == endStates:
        return True

    return False

def isWellSpecified(automata): #Funkce, ktera zjisti, jestli je automa dobre specifikovany
    states = automata[0]
    rules = automata[2]
    startState = automata[3]
    endStates = automata[4]
    availableStates = []
    newState = True
    availableStates.append(startState)
    notFinishing = 0


    while newState:
        newState = False
        for r in rules:
            start = r[0]
            end = r[2]
            if start not in availableStates:
                continue
            if end not in availableStates:
                availableStates.append(end)
                newState = True
                break

    if not compare(states, availableStates):
        error('Zadany automat neni dobre specifikovany!', 62)

    for s in states:
        pom = []
        for r in rules:
            if s in r[0]:
                pom.append(r)
        if not onlyOneChar(pom):
            error('Zadany automat neni dobre specifikovany!', 62)


    for s in states:
        if not isFinishing(s, rules, endStates, []):
            notFinishing += 1

    if notFinishing > 1:
        error('Zadany automat neni dobre specifikovany!', 62)


def minimize(automata): #Funkce pro minimalizaci konecneho automatu
    states = automata[0]
    alphabet = automata[1]
    rules = automata[2]
    startState = automata[3]
    endStates = automata[4]
    otherStates = []
    miniStates = []
    fission = True

    for s in states:
        if s not in endStates:
            otherStates.append(s)

    miniStates.append(endStates)
    miniStates.append(otherStates)

    while fission:
        fission = False
        for x in miniStates:
            for a in alphabet:
                pomRules = findRules(a, x, rules)
                pomStates = endingStates(pomRules)
                if len(pomStates) != 1:
                    if not isSameSet(pomStates, miniStates):
                        fission = True
                        miniStates = fiss(miniStates, pomStates, x, pomRules)

    #Zmena petice po minimalizaci, pro pripravu na vypis
    automata[0] = changeStates(miniStates)
    automata[2] = changeRules(miniStates, rules, automata[0])
    automata[3] = changeStartState(startState, miniStates, automata[0])
    automata[4] = automata[0][0]

    return automata

def onlyOneChar(rules):
    pom = []

    for r in rules:
        if r[1] in pom:
            return False
        pom.append(r[1])

    return True

def findRules(char, states, rules): #Najde pravidla, ve kterych se nachazi dane stavy
    findedRules = []

    for s in states:
        for r in rules:
            if s in r[0]:
                if char in r[1]:
                    findedRules.append(r)

    return findedRules

def endingStates(rules): #Vrati vsechny stavy, ktere konci v danych pravidlech
    states = []

    for r in rules:
        if r[2] not in states:
            states.append(r[2])

    return states

def isSameSet(states, miniStates): #Zjisti, jestli jsou vsechny stavy ze stejne mnoziny stavu
    i = 0

    for x in miniStates:
        if states[0] in x:
            break
        i += 1

    for s in states:
        if s not in miniStates[i]:
            return False

    return True

def fiss(miniStates, states, set, rules): #Funkce, ktera provede stepeni stavu
    pom = []

    for r in rules:
        if r[2] not in set:
            pom.append(r[0])

    miniStates.remove(set)

    for p in pom:
        set.remove(p)

    miniStates.append(pom)
    miniStates.append(set)

    return miniStates


def changeStates(miniStates): #Prejmenuje stavy na minimalizovane
    states = []
    symbol = ''

    for m in miniStates:
        for s in m:
            symbol += s
            if m.index(s) != len(m)-1:
                symbol += '_'

        states.append(symbol)
        symbol = ''

    return states

def changeRules(miniStates, rules, changedStates): #Prejmenuje pravidla na minimalizovane
    findedRules = []
    pom = []

    for r in rules:
        first = r[0]
        second = r[1]
        last = r[2]
        i = 0

        for x in miniStates:
            if r[0] in x:
                first = changedStates[i]
            i += 1

        i = 0
        for x in miniStates:
            if r[2] in x:
                last = changedStates[i]
            i += 1

        pom.append(first)
        pom.append(second)
        pom.append(last)
        if pom not in findedRules:
            findedRules.append(pom)
        pom = []

    return findedRules

def changeStartState(startState, miniStates, states): #Prejmenuje Startovaci stav na minimalizovany
    change = ''
    i = 0

    for x in miniStates:
        if startState in x:
            change = states[i]
            break
        i += 1

    return change

def findNonFinishing(automata, file): #Funkce, ktera nalezne neukoncuji stav
    states = automata[0]
    rules = automata[2]
    endStates = automata[4]

    if compare(states, endStates):
        file.write('0')

    for s in states:
        if not isFinishing(s, rules, endStates, []):
            file.write(s)
            return

    file.write('0')

def isFinishing(state, rules, endStates, yet): #Funkce, ktera zjisti, jestli je dany stav ukoncujici
    end = []

    for r in rules:
        if state in r[0]:
            if r[2] not in end and r[2] != state and r[2] not in yet:
                end.append(r[2])

    if len(end) == 0:
        return False

    for s in end:
        if s in endStates:
            return True

    for s in end:
        yet.append(s)
        if isFinishing(s, rules, endStates, yet):
            return True

    return False

def normalizedOutput(file, automata): #Vypsani automatu do souboru
    states = automata[0]
    alphabet = automata[1]
    rules = automata[2]
    startState = automata[3]
    endStates = automata[4]

    if isinstance(states, list):
        states.sort()

    if isinstance(endStates, list):
        endStates.sort()

    rules.sort(key=lambda x: (x[0], x[1]))

    file.write('(\n')
    file.write('{')
    if isinstance(states, list):
        statesOutput(states, file)
    else:
        file.write(states)
    file.write('},\n')
    file.write('{')
    alphabetOutput(alphabet, file)
    file.write('},\n')
    file.write('{\n')
    rulesOutput(rules, file)
    file.write('\n},\n')
    file.write(startState)
    file.write(',\n')
    file.write('{')
    if isinstance(endStates, list):
        statesOutput(endStates, file)
    else:
        file.write(endStates)
    file.write('}\n)\n')

def statesOutput(states, file): #Vypise stavy do slozenych zavorek
    for s in states:
        file.write(s)
        if states.index(s) != len(states)-1:
            file.write(', ')

def alphabetOutput(alphabet, file): #Vypise vstupni abecedu do slozenych zavorek
    for a in alphabet:
        file.write("'")
        if a != 'empty':
            file.write(a)
        file.write("'")
        if alphabet.index(a) != len(alphabet)-1:
            file.write(', ')

def rulesOutput(rules, file): #Vypise pravidla do souboru
    for r in rules:
        file.write(r[0])
        file.write(" '")
        if r[1] != 'empty':
            file.write(r[1])
        file.write("' -> ")
        file.write(r[2])
        if rules.index(r) != len(rules)-1:
            file.write(',\n')

def error(err, errorCode): #Vypise chybu na stderr a ukonci skript danym navratovym kodem
    sys.stderr.write(err + '\n')
    sys.exit(errorCode)

def printHelp(parser): #Vypise napovedu
    parser.print_help()
    sys.exit(0)

if __name__ == '__main__':
    main()