package model.board;

import model.cards.*;


/**
 * projekt: Solitaire Klondike - IJA 2016/2017
 * autori: Roman Nahalka (xnahal01), Adam Zivcak (xzivca03)
 * dátum: 6.5.2017
 * <p>
 * Abstraktná továreň na vytvorenie základných balíčkov.
 */

public abstract class AbstractFactorySolitaire {

    /**
     * Vytvorí balíček kariet, ktorý obsahuje 52 kariet - 13 kariet 4 farieb.
     * @return Vytvorený balíček.
     */
    public abstract CardDeck createCardDeck();

    /**
     * Vytvori objekt reprezentujuci kartu.
     * @param color Farba karty
     * @param value Hodnota karty
     * @return
     */
    public abstract Card createCard(Card.Color color, int value);

    /**
     * Vytvorí objekt reprezentujúci cieľový balíček.
     * @param color Farba cieľového balíčka.
     * @return Vytvorený balíček
     */
    public abstract CardDeck createTargetPack(Card.Color color);

    /**
     * Vytvorí objekt reprezentujúci pracovný balíček.
     * @return Vytvorený balíček.
     */
    public abstract CardStack createWorkingPack();

}
