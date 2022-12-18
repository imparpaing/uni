import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.imgcodecs.Imgcodecs;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.Buffer;

public class Lab3 {

    // Image path
    String imgPath = "/home/zorin/Documents/git/uni/Sem III/Image-processing/img/IMG_2116.jpg";

    // Load image from path, add reading color.
    Mat src = Imgcodecs.imread(imgPath, Imgcodecs.IMREAD_COLOR);

    // Create destination matrix
    Mat dest = new Mat(src.cols(), src.rows(), src.type());

    private BufferedImage createImage(Mat image) throws IOException {
        MatOfByte matOfByte = new MatOfByte();
        Imgcodecs.imencode(".jpg", image, matOfByte);
        byte[] byteArray = matOfByte.toArray();
        InputStream in = new ByteArrayInputStream(byteArray);
        return ImageIO.read(in);
    }

    private void makeJFrame(BufferedImage image) {
        // Initiate JFrame
        JFrame jFrame = new JFrame();
        jFrame.getContentPane().add(new JLabel(new ImageIcon(image)));
        jFrame.pack();
        jFrame.setVisible(true);

        // Exit program on window close
        jFrame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
    }

    // Exc 1 - Change image contrast
    public void changeImageContrast(int setAlpha) throws IOException {
        // Alter image alpha to change the contrast
        src.convertTo(dest, -1, 2, 0);

        // Display processed image
        BufferedImage buf = createImage(dest);
        makeJFrame(buf);
    }

    // Exc 2 - Change image brightness
    public void changeImageBrightness(int setBeta) throws IOException {
        // Alter image beta to change the brightness
        src.convertTo(dest, -1, 1, setBeta);

        // Display processed image
        BufferedImage buf = createImage(dest);
        makeJFrame(buf);
    }

    // Exc 3 - Overlay images
    public void overlayImages() throws IOException {
        // Create image paths
        String baseImage = imgPath;
        String stackImage = "/home/zorin/Documents/git/uni/Sem III/Image-processing/img/overlay.jpg";

        // Create image matrices
        Mat base = Imgcodecs.imread(baseImage);
        Mat stack = Imgcodecs.imread(stackImage);

        // Overlay the images
        Core.addWeighted(base, 0.8, stack, 0.8, 0.0, dest);

        // Display results
        BufferedImage buf = createImage(dest);
        makeJFrame(buf);
    }

}