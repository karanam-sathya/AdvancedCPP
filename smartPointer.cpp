#include <iostream>
#include <memory>
#include <vector>

class Sensor {
public:
    virtual void readData() = 0;
    virtual ~Sensor() = default;
};

class ECGSensor : public Sensor {
public:
    void readData() override {
        std::cout << "Reading ECG data..." << std::endl;
    }
    ~ECGSensor() {
        std::cout << "ECGSensor destroyed." << std::endl;
    }
};

class SpO2Sensor : public Sensor {
public:
    void readData() override {
        std::cout << "Reading SpO2 data..." << std::endl;
    }
    ~SpO2Sensor() {
        std::cout << "SpO2Sensor destroyed." << std::endl;
    }
};

class PatientMonitor {
private:
    // Sensors owned uniquely by the monitor
    std::vector<std::unique_ptr<Sensor>> uniqueSensors;

    // Sensors shared across multiple monitors
    std::vector<std::shared_ptr<Sensor>> sharedSensors;

    // Weak references to shared sensors (non-owning)
    std::vector<std::weak_ptr<Sensor>> weakSensors;

public:
    void addUniqueSensor(std::unique_ptr<Sensor> sensor) {
        uniqueSensors.push_back(std::move(sensor));
    }

    void addSharedSensor(std::shared_ptr<Sensor> sensor) {
        sharedSensors.push_back(sensor);
        weakSensors.push_back(sensor); // keep a weak reference too
    }

    void collectData() {
        std::cout << "\nCollecting data from unique sensors:" << std::endl;
        for (auto& sensor : uniqueSensors) {
            sensor->readData();
        }

        std::cout << "\nCollecting data from shared sensors:" << std::endl;
        for (auto& sensor : sharedSensors) {
            sensor->readData();
        }

        std::cout << "\nChecking weak sensors:" << std::endl;
        for (auto& weak : weakSensors) {
            if (auto locked = weak.lock()) {
                locked->readData();
            } else {
                std::cout << "Sensor no longer exists." << std::endl;
            }
        }
    }
};

int main() {
    PatientMonitor monitor;

    // Unique ownership: monitor fully controls lifecycle
    monitor.addUniqueSensor(std::make_unique<ECGSensor>());
    monitor.addUniqueSensor(std::make_unique<SpO2Sensor>());

    // Shared ownership: multiple monitors can share same sensor
    auto sharedECG = std::make_shared<ECGSensor>();
    monitor.addSharedSensor(sharedECG);

    // Another monitor sharing the same sensor
    PatientMonitor backupMonitor;
    backupMonitor.addSharedSensor(sharedECG);

    // Collect data
    monitor.collectData();
    backupMonitor.collectData();

    return 0;
}
