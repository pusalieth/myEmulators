
/*
 * GLPv3
 */

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import javax.swing.JFrame;

/**
 *
 * @author Jake Pring
 */
public class Chip8_Frame extends JFrame implements KeyListener {

    private Chip8_Panel panel;
    private int scale = 0;
    private int[] keyBuffer;
    private int[] keyIDToKey;

    public Chip8_Frame(Hardware object) {
        scale = object.getScale();
        int x = 64 * scale;
        int y = 32 * scale;
        setPreferredSize(new Dimension(x, y));
        pack();
        y += getInsets().top + getInsets().bottom;
        x += getInsets().left + getInsets().right;
        setPreferredSize(new Dimension(x, y));
        pack();
        panel = new Chip8_Panel(object);
        setLayout(new BorderLayout());
        add(panel, BorderLayout.CENTER);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setTitle("Chip-8 Emulator");
        pack();
        setVisible(true);
        addKeyListener(this);

        keyIDToKey = new int[256];
        keyBuffer = new int[16];
        fillKeyIDs();
    }

    public void fillKeyIDs() {
        for (int i = 0; i < keyIDToKey.length; i++) {
            keyIDToKey[i] = -1;
        }
        keyIDToKey['1'] = 1;
        keyIDToKey['2'] = 2;
        keyIDToKey['3'] = 3;
        keyIDToKey['Q'] = 4;
        keyIDToKey['W'] = 5;
        keyIDToKey['E'] = 6;
        keyIDToKey['A'] = 7;
        keyIDToKey['S'] = 8;
        keyIDToKey['D'] = 9;
        keyIDToKey['Z'] = 0xA;
        keyIDToKey['X'] = 0;
        keyIDToKey['C'] = 0xB;
        keyIDToKey['4'] = 0xC;
        keyIDToKey['R'] = 0xD;
        keyIDToKey['F'] = 0xE;
        keyIDToKey['V'] = 0xF;
    }

    @Override
    public void keyPressed(KeyEvent e) {
        int keyReg = keyIDToKey[e.getKeyCode()];
        if (keyReg != -1) {
            keyBuffer[keyReg] = 1;
        }
    }

    @Override
    public void keyReleased(KeyEvent e) {
        int keyReg = keyIDToKey[e.getKeyCode()];
        if (keyReg != -1) {
            keyBuffer[keyReg] = 0;
        }
    }

    @Override
    public void keyTyped(KeyEvent e) {
    }

    public int[] getKeyBuffer() {
        return keyBuffer;
    }
}
