package graphics;

import model.board.Game;
import model.cards.CardDeck;

/**
 * projekt: Solitaire Klondike - IJA 2016/2017
 * autori: Roman Nahalka (xnahal01), Adam Zivcak (xzivca03)
 * dátum: 6.5.2017
 *
 * Rozhranie pre pracu s balickami typu deck.
 */
public interface DeckPanelInterface {

    /**
     * Vrati balicek karet, prirazeny k jeho graficke reprezentaci.
     * @return Balicek karet
     */
    CardDeck getDeck();

    /**
     * Nastavi cilovi balicek a hru, ke ktere patri.
     * @param deck Balicek karet
     * @param game Hra, ke ktere balicek patri
     */
    void setDeck(CardDeck deck, Game game);
}
