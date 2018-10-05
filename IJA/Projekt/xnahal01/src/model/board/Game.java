package model.board;

import java.io.File;
import java.util.Stack;

import model.cards.CardDeck;
import model.cards.CardStack;

/**
 * projekt: Solitaire Klondike - IJA 2016/2017
 * autori: Roman Nahalka (xnahal01), Adam Zivcak (xzivca03)
 * dátum: 6.5.2017
 * <p>
 * Rozhranie pre hru.
 */
public interface Game {

    /**
     * @return Aktuálnu hru.
     */
    Game getGame();

    /**
     * Vráti pracovný balíček s daným indexom.
     * @param index Pozícia balíčku.
     * @return Pracovný balíček.
     */
    CardStack getStack(int index);

    /**
     * Vráti cieľový balíček s daným indexom.
     * @param index Pozícia balíčku.
     * @return Cieľový balíček.
     */
    CardDeck getTarget(int index);

    /**
     * @return Hlavný balíček, z ktorého sa ťahajú karty.
     */
    CardDeck getMainDeck();

    /**
     * @return Balíček, na ktorý sa odkladajú karty potihnuté z MainDecku.
     */
    CardDeck getWasteDeck();

    /**
     * Získa nápovedu pre vykonanie ťahu.
     * @return Textová nápoveda pre vykonanie možného ťahu.
     */
    String getHint();

    /**
     * Potiahne kartu z MainDeck na WasteDeck ak sú na MainDeck nejaké karty.
     * Ak nie sú, tak dôjde k preloženiu kariet z WasteDeck na MainDeck.
     * @return True, ak bol MainDeck prázdny a došlo k prekladaniu kariet z WasteDecku. Inak false.
     */
    boolean dealCard();

    /**
     * Inicializácia hry.
     * Rozloží (rozdá) karty na hrací plán a vytvorí cieľové balíčky.
     */
    void laidCards();

    /**
     * Uloží aktuálnu hru do súboru.
     * @param game Aktuálna hra.
     * @param file Súbor, kde sa hra uloží.
     */
    void saveGame(Game game, File file);

    /**
     * Načíta hru zo súboru.
     * @param file Súbor s rozohranou hrou.
     * @return Rozohranú hru.
     */
    Game loadGame(File file);

    /**
     * Vloží vykonaný ťah na zásobník ťahov.
     * @param i Ťah, ktorý sa uloží.
     */
    void pushItem(UndoStackItem i);

    /**
     * Odstráni posledný ťah zo zásbnika ťahov.
     * @return Odstránený ťah.
     */
    UndoStackItem popItem();

    /**
     * Overí, či hra ešte nie je dohratá.
     * @return True, ak je hra dohratá.
     */
    boolean checkWin();

    /**
     * Aktualizuje skóre podľa vykonaného ťahu.
     * @param from Miesto odkiaľ sme brali kartu/karty.
     * @param to Miesto kam sme položili kartu/karty.
     */
    void scoring(int from, int to);

    /**
     * Aktualizuje skóre na základe nejakej udalosti (otočenie karty, otočenie hlavného balíčka).
     * @param type Typ udalosti.
     */
    void scoring(int type);

    /**
     * @return Aktuálne skóre hry.
     */
    int getScore();

    /**
     * Nastaví aktuálne skóre hry na zadanú hodnotu.
     * @param score Hodnota skóre, ktorá bude nastavená.
     */
    void setScore(int score);

    /**
     * K aktualnému skóre pripočíta zadanú hodnotu.
     * Ak by výsledná hodnota bola menšia ako 0, tak nastaví hodnotu 0.
     * @param value Hodnota, ktorá bude pripočitaná k aktualnej hodnote skóre.
     */
    void addScore(int value);

    /**
     * Nastaví zásobník pre ukladanie vykonaných ťahov
     */
    void setUndoStack();
}