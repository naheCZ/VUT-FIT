package model.board;

import model.cards.*;


/**
 * project: project Solitaire model.Klondike - IJA 2016/2017
 * authors: Adam Zivcak, Roman Nahalka
 * login:   xzivca03, xnahal01
 * created: 30.3.2017
 */

public abstract class AbstractFactorySolitaire {

    // Vytváří objekt reprezentující balíček karet.
    public abstract CardDeck createCardDeck();

    // Vytvoří objekt reprezentující kartu.
    public abstract Card createCard(Card.Color color, int value);

    // Vytváří objekt reprezentující cílový balíček.
    public abstract CardDeck createTargetPack(Card.Color color);

    // Vytváří objekt reprezentující pracovní pole pro karty.
    public abstract CardStack createWorkingPack();

}
