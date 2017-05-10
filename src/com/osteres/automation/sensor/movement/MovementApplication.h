//
// Created by Thibault PLET on 08/05/2017.
//

#ifndef COM_OSTERES_AUTOMATION_SENSOR_MOVEMENT_MOVEMENTAPPLICATION_H
#define COM_OSTERES_AUTOMATION_SENSOR_MOVEMENT_MOVEMENTAPPLICATION_H

/* Defined values */
#define IGNORE_PACKET_SUCCESS_RESPONSE true


#include <Arduino.h>
#include <com/osteres/automation/arduino/ArduinoApplication.h>
#include <com/osteres/automation/sensor/Identity.h>
#include <com/osteres/automation/sensor/movement/action/ActionManager.h>
#include <com/osteres/automation/sensor/movement/action/TransmitMovement.h>
#include <com/osteres/automation/arduino/memory/PinProperty.h>

using com::osteres::automation::arduino::ArduinoApplication;
using com::osteres::automation::sensor::Identity;
using com::osteres::automation::sensor::movement::action::ActionManager;
using com::osteres::automation::sensor::movement::action::TransmitMovement;
using com::osteres::automation::arduino::memory::PinProperty;

namespace com
{
    namespace osteres
    {
        namespace automation
        {
            namespace sensor
            {
                namespace movement
                {
                    class MovementApplication : public ArduinoApplication
                    {
                    public:
                        /**
                         * Sensor identifier
                         */
                        static byte const SENSOR = Identity::MOVEMENT;

                        /**
                         * Constructor
                         */
                        MovementApplication(
                            Transmitter *transmitter,
                            unsigned int unlockShutdownPin
                        ) : ArduinoApplication(MovementApplication::SENSOR, transmitter)
                        {
                            this->construct(unlockShutdownPin);
                        }

                        /**
                         * Destructor
                         */
                        virtual ~MovementApplication()
                        {
                            // Remove movement action
                            if (this->actionMovement != NULL) {
                                delete this->actionMovement;
                                this->actionMovement = NULL;
                            }
                            // Remove unlock shutdown property
                            if (this->unlockShutdownProperty != NULL) {
                                delete this->unlockShutdownProperty;
                                this->unlockShutdownProperty = NULL;
                            }
                        }

                        /**
                         * Setup application
                         */
                        virtual void setup()
                        {
                            // Parent
                            ArduinoApplication::setup();

                            // Ensure that arduino keep alive (power supply)
                            this->getUnlockShutdownProperty()->set(0);

                            // Transmission
                            this->transmitter->setActionManager(this->getActionManager());
                        }

                        /**
                         * Process application
                         */
                        virtual void process()
                        {
                            // Request an identifier if needed. Note: Not mandatory anymore
                            if (this->isNeedIdentifier()) {
                                this->requestForAnIdentifier();

                                // Send and listen
                                this->transmitter->srs(3000); // 3s

                            } // Process
                            else {

                                // Send movement signal
                                this->requestForSendData();

                                // Send battery level
                                this->requestForBatteryLevel();

                                // Send and listen
                                this->transmitter->srs();

                                // Now, arduino can be turned off
                                this->getUnlockShutdownProperty()->set(1);
                                // From here, potentially, arduino shutdown
                            }

                            // Wait 100ms
                            delay(100);
                        }

                        /**
                         * Send data to server
                         */
                        void requestForSendData()
                        {
                            // Process
                            this->getActionMovement()->execute();
                        }

                        /**
                         * Get action movement
                         */
                        TransmitMovement *getActionMovement()
                        {
                            if (this->actionMovement == NULL) {
                                this->actionMovement = new TransmitMovement(
                                    this->getPropertyType(),
                                    this->getPropertyIdentifier(),
                                    Identity::MASTER,
                                    this->transmitter
                                );
                            }

                            return this->actionMovement;
                        }

                        /**
                         * Get unlock shutdown property
                         * If false, arduino keep alive, otherwise power can be cut off
                         */
                        PinProperty<unsigned int> * getUnlockShutdownProperty()
                        {
                            return this->unlockShutdownProperty;
                        }

                    protected:

                        /**
                         * Common part constructor
                         */
                        void construct(unsigned int unlockShutdownPin)
                        {
                            // Create action manager
                            ActionManager *actionManager = new ActionManager();
                            this->setActionManager(actionManager);

                            // Unlock shutdown property
                            this->unlockShutdownProperty = new PinProperty<unsigned int>(unlockShutdownPin, true, false);
                        }

                        /**
                         * Action to transmit movement
                         */
                        TransmitMovement *actionMovement = NULL;

                        /**
                         * Unlock shutdown property
                         * If 0, arduino keep alive, if 1 power can be cut off
                         */
                        PinProperty<unsigned int> * unlockShutdownProperty = NULL;
                    };
                }
            }
        }
    }
}


#endif //COM_OSTERES_AUTOMATION_SENSOR_MOVEMENT_MOVEMENTAPPLICATION_H
