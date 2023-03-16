import java.util.*;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public class Main {
    public static void main(String[] args) {
        System.out.println("Stream API");

        System.out.println("--------------");

        // Get the smallest element of storeIntegers
        System.out.println("Get smallest element:");
        ArrayList<Integer> storeIntegers = new ArrayList<>();
        storeIntegers.add(7);
        storeIntegers.add(14);
        storeIntegers.add(21);

        Stream<Integer> smallestInteger = storeIntegers.stream();
        System.out.println(smallestInteger.min(Integer::compare));

        System.out.println("--------------");

        // Get even elements of storeIntegers
        System.out.println("Get even elements:");
        Stream<Integer> evenIntegers = storeIntegers.stream();
        evenIntegers.filter(integer -> integer % 2 == 0).forEach(System.out::println);

        System.out.println("--------------");

        // Sort Person class objects
        System.out.println("Order class objects:");
        Person person1 = new Person("Nick", 18);
        Person person2 = new Person("Maria", 21);
        Person person3 = new Person("Ash", 17);

        ArrayList<Person> people = new ArrayList<>();
        people.add(person1);
        people.add(person2);
        people.add(person3);

        Stream<Person> personStream = people.stream();

        personStream.sorted(Comparator.comparing(Person::getNick).thenComparing(Person::getAge))
                .forEach(person -> System.out.println("Name: " + person.getNick() + " Age: " +  person.getAge()));

        System.out.println("--------------");

        // Remove 'Z' coordinate
        System.out.println("Map XYZ point to XY point ");
        PointXYZ pointXYZ1 = new PointXYZ(13, 7, 8);
        PointXYZ pointXYZ2 = new PointXYZ(21, 4, 5);
        PointXYZ pointXYZ3 = new PointXYZ(33, -13, 17);

        ArrayList<PointXYZ> points3d = new ArrayList<>();
        points3d.add(pointXYZ1);
        points3d.add(pointXYZ2);
        points3d.add(pointXYZ3);

        Stream<PointXYZ> pointXYZStream = points3d.stream();
        pointXYZStream.map(pointXYZ -> new PointXY(pointXYZ.getX(), pointXYZ.getY()))
                .collect(Collectors.toCollection(ArrayList::new))
                .forEach(pointXY -> System.out.println("X: " + pointXY.getX() + ", Y: " + pointXY.getY()));
    }
}