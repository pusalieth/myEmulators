/*
 * GPLv3
 */
package Chip8;

import java.awt.Color;
import java.awt.Graphics;
import javax.swing.JPanel;

/**
 *
 * @author Jake Pring
 */
public class Chip8_Panel extends JPanel {
    
    private Hardware chip8;
    private int scale = 0;

    public Chip8_Panel(Hardware object) {
        chip8 = object;
        scale = chip8.getScale();
    }

    public void paint(Graphics g) {
        byte[][] display = chip8.getDisplay();
        for (int x = 0; x < 64; x++) {
            for (int y = 0; y < 32; y++) {
                if (display[x][y] == 1)
                    g.setColor(Color.WHITE);
                else
                    g.setColor(Color.BLACK);
                
                g.fillRect(x*scale, y*scale, scale, scale);
            }
        }
        
        /*
        for (int i = 0; i < display.length; i++) {
            if (display[i] == 0) {
                g.setColor(Color.BLACK);
            } else {
                g.setColor(Color.WHITE);
            }
            int x = (i%64);
            int y = (int) Math.floor(i / 64);
            g.fillRect(x*scale, y*scale, scale, scale);
        }*/
    }
}
