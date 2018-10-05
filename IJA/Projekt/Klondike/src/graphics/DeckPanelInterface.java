package graphics;

import model.board.Game;
import model.cards.CardDeck;

/**
 * project: Klondike (copy)
 * author: Adam Zivcak
 * login:  xzivca03
 * created: 8.4.2017
 */
public interface DeckPanelInterface {

    CardDeck getDeck();

    void setDeck(CardDeck deck, Game game);
}
