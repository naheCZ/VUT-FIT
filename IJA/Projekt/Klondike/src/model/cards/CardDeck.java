package model.cards;

import java.util.ArrayList;

/**
 * project: project Solitaire model.Klondike - IJA 2016/2017
 * authors: Adam Zivcak, Roman Nahalka
 * login:   xzivca03, xnahal01
 * created: 30.3.2017
 */

public interface CardDeck {

    /**
     * Vrátí kartu z vrcholu zásobníku (karta zůstává na zásobníku).
     * Pokud je balíček prázdný, vrací null.
     * @return Karta z vrcholu balíčku.
     */
    Card get();

    /**
     * Vrátí kartu na uvedenem indexu.
     * Spodni karta je na indexu 0, vrchol je na indexu size()-1.
     * Pokud je balíček prázdný, nebo index mimo rozsah, vrací null.
     * @param index Pozice karty v balicku.
     * @return Karta z vrcholu balíčku.
     */
    Card get(int index);

    /**
     * Test, zda je balíček karet prázdný.
     * @return Vrací true, pokud je balíček prázdný.
     */
    boolean isEmpty();

    /**
     * Odebere kartu z vrcholu balíčku. Pokud je balíček prázdný, vrací null.
     * @return Karta z vrcholu balíčku.
     */
    Card pop();

    /**
     * Vloží kartu na vrchol balíčku.
     * @param card Vkládaná karta.
     * @return Úspěšnost akce.
     */
    boolean put(Card card);


    /**
     * @return Aktuální počet karet v balíčku.
     */
    int size();

    void put(Card.Color c, int value);

    void shuffleCards();

    boolean checkPut(Card card);

    /**
     * @return true ak sa otočenie vykonalo
     */
    boolean rotateLastCard();

    void forceRotate();
    
    ArrayList<Card> getDeck();


}
