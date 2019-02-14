
/*
 * GPLv3
 */
import java.io.File;
import java.util.Scanner;

/**
 *
 * @author Jake Pring
 */
public class Main extends Thread {

    private Hardware chip8;
    private Chip8_Frame frame;

    public Main(String argv) {
        chip8 = new Hardware();

        if (argv.contains("debug")) {
            
        } else {
            // Normal Display Dimensions are 64x32
            System.out.println("Please select a resolution");
            for (int i = 1; i <= 12; i++) {
                System.out.print(i + ". \"" + (64 * (i + 9)) + "x" + (32 * (i + 9)) + "\"");
                System.out.print("\t");
                i++;
                System.out.println(i + ". \"" + (64 * (i + 9)) + "x" + (32 * (i + 9)) + "\"");
            }

            System.out.println();
            Scanner input = new Scanner(System.in);
            int scale = input.nextInt();
            chip8.setScale(scale + 9);
        }

        chip8.init();

        // List Programs
        if (argv.charAt(0) != '/') {
            System.out.println("Please select a game File");
            File folder = new File(argv);
            File[] listOfFiles = folder.listFiles();
            String[] listOfGames = new String[30];

            int x = 0;
            for (int i = 0; i < listOfFiles.length; i++) {
                String currentFileName = listOfFiles[i].getName();
                int fileNameLength = currentFileName.length();
                String fileExtension = currentFileName.substring(fileNameLength - 3, fileNameLength);
                if (listOfFiles[i].isFile() && (".c8".equals(fileExtension))) {
                    listOfGames[x] = currentFileName;
                    x++;
                    System.out.print(x + ". " + currentFileName);
                    if (x % 2 == 0) {
                        System.out.println();
                    } else {
                        System.out.print("\t");
                    }
                }

            }

            System.out.println();
            Scanner input2 = new Scanner(System.in);
            int fileChoice = input2.nextInt();
            System.out.println("Loading Game: " + listOfGames[fileChoice] + "....");

            chip8.loadProgram(argv + listOfGames[fileChoice - 1]);
        } else {
            chip8.loadProgram(argv);
        }

        frame = new Chip8_Frame(chip8);
    }

    public void run() {
        long i = 0;
        boolean onChange_Draw = false;
        while (true) {
            chip8.setKeyBuffer(frame.getKeyBuffer());
            chip8.CPU_run();

            if (chip8.needsReDraw()) {
                if (onChange_Draw) {
                    frame.repaint();
                }
                onChange_Draw ^= true;
                chip8.removeDrawFlag();
            }

            if (true) {
                i = 0;
                try {
                    Thread.sleep(1);
                } catch (InterruptedException e) {
                    //
                }
            } else {
                try {
                    Thread.sleep(10);
                } catch (InterruptedException e) {
                    //
                }
            }
            i++;
        }
    }

    public static void main(String[] args) {
        Main main = new Main(args[0]);
        main.start();
    }
}
