package model.cards;

import java.util.ArrayList;

/**
 * projekt: Solitaire Klondike - IJA 2016/2017
 * autori: Roman Nahalka (xnahal01), Adam Zivcak (xzivca03)
 * dátum: 6.5.2017
 * <p>
 * Rozhranie pre prácu s CardDeckom.
 */

public interface CardDeck {

    /**
     * Vráti kartu z vrcholu zásobníku (karta zostáva na zásobníku).
     * Ak je balíček prázdny, vracia null.
     *
     * @return Karta z vrcholu balíčka.
     */
    Card get();

    /**
     * Vrátí kartu na uvedenom indexe.
     * Spodná karta je na indexe 0, vrchol je na indexe size()-1.
     * Ak je balíček prázdny, alebo je index mimo rozsah, vracia null.
     *
     * @param index Pozícia karty v balíčku.
     * @return Karta z vrcholu balíčku.
     */
    Card get(int index);

    /**
     * Test, či je balíček kariet prázdný.
     *
     * @return Vracia true, ak je balíček prázdny.
     */
    boolean isEmpty();

    /**
     * Odeberie kartu z vrcholu balíčka. Ak je balíček prázdny, vracia null.
     *
     * @return Karta z vrcholu balíčku.
     */
    Card pop();

    /**
     * Overí, či je možné položiť kartu na daný balíček.
     *
     * @param card Vkladaná karta
     * @return True, ak je možné kartu položiť na balíček.
     */
    boolean checkPut(Card card);

    /**
     * Vloží kartu na vrchol balíčku.
     *
     * @param card Vkladaná karta.
     * @return Úspešnosť akcie.
     */
    boolean put(Card card);

    /**
     * Vloží kartu na vrchol aehokoľvek balíčku (neoveruje žiadne podmienky).
     *
     * @param c     Farba vkladanej karty.
     * @param value Hodnota vkladanej karty.
     */
    void put(Card.Color c, int value);

    /**
     * @return Aktuálny počet kariet v balíčku.
     */
    int size();

    /**
     * Zamieša karty v balíčku.
     */
    void shuffleCards();

    /**
     * Otočí poslednú kartu v balíčku.
     *
     * @return True ak sa otočenie vykonalo.
     */
    boolean rotateLastCard();

    /**
     * @return Balíček kariet.
     */
    ArrayList<Card> getDeck();
}
