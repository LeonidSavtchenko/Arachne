// Background mode utility methods

void ReportCurrentProgressBackground(int iter, int m_steps, double iterDur, bool continuationMode, int m_steps_prev);

bool CheckIfTerminationRequested();

bool CheckIfSnapshotRequested();

// Backup file utility methods

bool GetTargetBackupStorage(bool continuationMode);

void SwitchTargetBackupStorage(bool switchTo1st);

void CopySnapshotFromBackupStorage(bool snapshotFrom1st);