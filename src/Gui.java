import javax.swing.*;
import java.awt.Dimension;
import java.awt.Image;

public class Gui{
    static int WIDTH = 1000, HEIGHT = 600;
    static String title = "Server Transfer";
    static String version = "0.1";
    static JFrame frame;
    static JPanel panel;
    public static void main(String[] args) {
        windowSetup();
    }

    private static void windowSetup(){
        ImageIcon icon = new ImageIcon("../assets/icon.png");

        frame = new JFrame(title + " " + version);
        panel = new JPanel();
        panel.setPreferredSize(new Dimension(WIDTH, HEIGHT));

        frame.add(panel);
        frame.pack();
        frame.setIconImage(icon.getImage());
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);
    }
}
