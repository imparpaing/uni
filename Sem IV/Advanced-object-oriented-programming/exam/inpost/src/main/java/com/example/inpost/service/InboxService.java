package com.example.inpost.service;

import com.example.inpost.models.Inbox;
import com.example.inpost.repos.InboxRepo;
import org.springframework.stereotype.Service;

import java.util.*;

@Service
public class InboxService {

    private final InboxRepo inboxRepo;

    public InboxService(InboxRepo inboxRepo) {
        this.inboxRepo = inboxRepo;
    }

    public List<Inbox> getAllInboxes() {
        return inboxRepo.findAll();
    }

    public Boolean checkInboxPin(Long inboxPin) {
        Long pin = inboxRepo.getInboxPin(inboxPin);
        if (pin == null) {
            return false;
        }
        return pin.equals(inboxPin);
    }

    public Inbox releasePackage(Long inboxPin) {
        Inbox inbox = inboxRepo.getInboxByPin(inboxPin);
        if (inbox != null && !inbox.getAvailable()) {
            inboxRepo.markInboxAsAvailable(inboxPin);
            return inbox;
        }
        return null;
    }

    public Inbox getSmallestInbox(String parcelSize) {
        List<Inbox> availableInboxes = inboxRepo.getSmallestInboxAvailable(parcelSize);
        if (!availableInboxes.isEmpty()) {
            // TODO: Set the parcel availability to false
            return availableInboxes.get(0);
        }
        return null;
    }

}
