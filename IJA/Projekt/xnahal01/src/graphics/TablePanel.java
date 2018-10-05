package graphics;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.Serializable;

/**
 * projekt: Solitaire Klondike - IJA 2016/2017
 * autori: Roman Nahalka (xnahal01), Adam Zivcak (xzivca03)
 * dátum: 6.5.2017
 * 
 * Graficka reprezentace herniho stolu, na kterem jsou umisteny vsechny balicky. 
 */
public class TablePanel extends JPanel implements Serializable
{
    private BufferedImage img;
    
    /**
     * Konstruktor, ktery vykresli grafickou reprezentaci stolu pomoci obrazkove
     * textury. 
     */
    public TablePanel()
    {
        setVisible(true);
        setOpaque(false);

        try
        {
            img = ImageIO.read(getClass().getResource("/images/Table.png"));
        }
        
        catch (IOException ex)
        {
            ex.printStackTrace();
        }
    }
    
    @Override
    protected void paintComponent(Graphics g)
    {
        int width;
        int height;
        
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g.create();
        
        width = img.getWidth();
        height = img.getHeight();
        
        for(int y = 0; y < getHeight(); y += height)
        {
            for(int x = 0; x < getWidth(); x += width)
                g2d.drawImage(img, x, y, null);
        }
        
        g2d.dispose();
    }
}
