package model.cards;

/**
 * project: project Solitaire model.Klondike - IJA 2016/2017
 * authors: Adam Zivcak, Roman Nahalka
 * login:   xzivca03, xnahal01
 * created: 30.3.2017
 */

public interface CardStack extends CardDeck {

    /**
     * Metoda odebere ze zásobníku sekvenci karet od zadané karty až po vrchol zásobníku.
     * Pokud je hledaná karta na vrcholu, bude v sekvenci pouze jedna karta.
     *
     * @param card Hledaná karta.
     * @return Zásobník karet obsahující odebranou sekvenci. Pokud hledaná karta v zásobníku není, vrací null.
     */
    CardStack pop(Card card);

    /**
     * Vloží karty ze zásobníku stack na vrchol zásobníku.
     * Karty vkládá ve stejném pořadí, v jakém jsou uvedeny v zásobníku stack.
     *
     * @param stack Zásobník vkládaných karet.
     * @return Uspěšnost akce.
     */
    boolean put(CardStack stack);

    boolean put2(CardStack stack);

    boolean checkPut(Card card);

    boolean rotateLastCard();

}