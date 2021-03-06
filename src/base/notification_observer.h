// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SRC_BASE_NOTIFICATION_OBSERVER_H_
#define SRC_BASE_NOTIFICATION_OBSERVER_H_

class NotificationDetails;
class NotificationSource;
class NotificationType;

// This is the base class for notification observers. When a matching
// notification is posted to the notification service, Observe is called.
class NotificationObserver {
 public:
  NotificationObserver();
  virtual ~NotificationObserver();

  virtual void Observe(NotificationType type,
                       const NotificationSource& source,
                       const NotificationDetails& details) = 0;
};

#endif  // SRC_BASE_NOTIFICATION_OBSERVER_H_
