package com.example.inpost.service;

import com.example.inpost.model.Package;
import com.example.inpost.repo.PackageRepo;
import org.springframework.stereotype.Service;

import java.sql.Date;
import java.util.Calendar;

@Service
public class PackageService {

    private final PackageRepo packageRepo;

    public PackageService(PackageRepo packageRepo) {
        this.packageRepo = packageRepo;
    }

    public void sendPackage(Package parcel) {
        String parcelType = parcel.getType();
        Calendar today = Calendar.getInstance();
        today.set(Calendar.HOUR_OF_DAY, 0);
        Date collectionDate;

        if (parcelType.equals("Short-term")) {
            // Set pick-up deadline for 2 days
            today.add(Calendar.DAY_OF_MONTH, 2);
        } else {
            // Set pick-up deadline for 7 days
            today.add(Calendar.DAY_OF_MONTH, 7);
        }

        collectionDate = new Date(today.getTime().getTime());
        parcel.setCollectionDeadline(collectionDate);
        packageRepo.save(parcel);
    }
}