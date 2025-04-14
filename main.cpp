/// THIS IS MAIN CPP

#include "inverse_header.h"
// #include "icecream.hpp"
// #include "/home/aldon/Downloads/polos2/note"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "cmath"
#include <iostream>

int main() {
    // Panggil fungsi siap() satu kali
    int dxl_comm_result = COMM_TX_FAIL; // Communication result
    uint8_t dxl_error = 0; // Dynamixel error

    // Open port
    if (portHandler->openPort())
    {
        printf("Succeeded to open the port!\n");
    }
    else
    {
        printf("Failed to open the port!\n");
        printf("Press any key to terminate...\n");
        return 0;
    }

    // Set port baudrate
    if (portHandler->setBaudRate(BAUDRATE))
    {
        printf("Succeeded to change the baudrate!\n");
    }
    else
    {
        printf("Failed to change the baudrate!\n");
        printf("Press any key to terminate...\n");
        return 0;
    }

    // Create a GroupSyncWrite instance for speed control
    dynamixel::GroupSyncWrite groupSyncWriteSpeed(portHandler, packetHandler, ADDR_AX_GOAL_SPEED, LEN_AX);
    
    // IC();
    for (int i = 0; i < 18; i++)
    {
        // Enable Dynamixel#1 Torque
        dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, dxl_idku[i], ADDR_AX_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
        if (dxl_comm_result != COMM_SUCCESS)
        {
            printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            printf("%s\n", packetHandler->getRxPacketError(dxl_error));
        }
        else
        {
            printf("Dynamixel#%d has been successfully connected \n", dxl_idku[i]);
        }

        // Set initial speed to very slow (modify the value as needed)
        // For AX series: 1 is slowest, 1023 is fastest, 0 is MAX speed
        uint16_t init_speed = ultra_fast; // Using your very_slow constant
        uint8_t speed_params[2];
        speed_params[0] = DXL_LOBYTE(init_speed);
        speed_params[1] = DXL_HIBYTE(init_speed);
        groupSyncWriteSpeed.addParam(dxl_idku[i], speed_params);
    }

    // Send speed commands to all servos before any movement
    dxl_comm_result = groupSyncWriteSpeed.txPacket();
    if (dxl_comm_result != COMM_SUCCESS) {
        printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
    }
    groupSyncWriteSpeed.clearParam();

    // Allow some time for speed settings to take effect
    usleep(50000); // 50ms delay

    siap();
    // usleep(2500000);
    // lift_front_legs();
    // usleep(2500000);
    // lift_middle_legs();
    // usleep(2500000);
    // lift_back_legs();
    // usleep(2500000);

    std::cout << "Tekan Enter untuk menjalankan coba_majuv2()..." << std::endl;

    while (true) { // Loop agar menunggu input terus-menerus
        std::cout << "Press 1 and Enter for coba_majuv2() or 2 and Enter for lift_middle_legs()..." << std::endl;
    
        char choice;
        std::cin >> choice;
        std::cin.ignore(); // Clear the newline
        
        if (choice == '1') {
            coba_majuv2();
        } 
        else if (choice == '2') {
            lift_front_legs();
        } 
        else if (choice == '3') {
            lift_middle_legs();
        }
        else if (choice == '4') {
            lift_back_legs();
        }
        else if (choice == '5') {
            translation_test();
        }
        
        
        std::cout << "Enter your next command..." << std::endl;
    }

    return 0;
}


// Void Functions 
void inverse_k(uint8_t dxl_id, float x, float y, float z, int kecepatan)
{
    // Make sure kecepatan is never 0 (which means maximum speed)
    if (kecepatan <= 0) kecepatan = 1;
    
    // Add debug output to verify speeds being set
    printf("Setting speed for servo %d to %d\n", dxl_id, kecepatan);

    dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, ADDR_AX_GOAL_POSITION, LEN_AX);
    dynamixel::GroupSyncWrite groupSyncWriteSpeed(portHandler, packetHandler, ADDR_AX_GOAL_SPEED, LEN_AX);

    //  a. HITUNG THETA Coxa
    //  hitung theta c
    theta_c = radToServomx(atan2(y, x));
    x0 = sqrt((y * y) + (x * x));
    // b. HITUNG THETA Femur
    // hitung theta f1
    x1 = x0 - c; // pengurangan panjang x0 dan coxa
    theta_f1 = radToServoax(atan2(z, x1));
    // hitung panjang a
    a = sqrt((z * z) + (x1 * x1));
    // hitung f2
    theta_f2 = radToServoax(acos((pow(f, 2) + pow(a, 2) - pow(t, 2)) / (2 * a * f)));
    // hitung f
    theta_f = theta_f1 + theta_f2;
    // c. HITUNG THETA Tibia
    // hitung theta t
    theta_t = radToServoax(acos((pow(f, 2) + pow(t, 2) - pow(a, 2)) / (2 * f * t)) - 1.57);

    // d. Normalisasi 0 derajat servo
    if (dxl_id == kR1 || dxl_id == kR2 || dxl_id == kR3)
    {
        theta_c_real = 2048 - theta_c;
        theta_f_real = 512 - theta_f;
        theta_t_real = 512 + theta_t;
    }

    if (dxl_id == kL1 || dxl_id == kL2 || dxl_id == kL3)
    {
        theta_c_real = 2048 - theta_c;
        theta_f_real = 512 - theta_f;
        theta_t_real = 512 + theta_t;
    }

    switch (dxl_id)
    {
    case kR1:
        posisi_servo[0] = theta_t_real;
        posisi_servo[1] = theta_f_real;
        posisi_servo[2] = theta_c_real;

        speed_servo[0] = kecepatan;
        speed_servo[1] = kecepatan;
        speed_servo[2] = kecepatan;

        R1_x = x;
        R1_y = y;
        R1_z = z;
        break;

    case kR2:
        posisi_servo[3] = theta_t_real;
        posisi_servo[4] = theta_f_real;
        posisi_servo[5] = theta_c_real;

        speed_servo[3] = kecepatan;
        speed_servo[4] = kecepatan;
        speed_servo[5] = kecepatan;

        R2_x = x;
        R2_y = y;
        R2_z = z;
        break;

    case kR3:
        posisi_servo[6] = theta_t_real;
        posisi_servo[7] = theta_f_real;
        posisi_servo[8] = theta_c_real;

        speed_servo[6] = kecepatan;
        speed_servo[7] = kecepatan;
        speed_servo[8] = kecepatan;

        R3_x = x;
        R3_y = y;
        R3_z = z;
        break;

    case kL1:
        posisi_servo[9] = theta_t_real;
        posisi_servo[10] = theta_f_real;
        posisi_servo[11] = theta_c_real;

        speed_servo[9] = kecepatan;
        speed_servo[10] = kecepatan;
        speed_servo[11] = kecepatan;

        L1_x = x;
        L1_y = y;
        L1_z = z;
        break;

    case kL2:
        posisi_servo[12] = theta_t_real;
        posisi_servo[13] = theta_f_real;
        posisi_servo[14] = theta_c_real;

        speed_servo[12] = kecepatan;
        speed_servo[13] = kecepatan;
        speed_servo[14] = kecepatan;

        L2_x = x;
        L2_y = y;
        L2_z = z;
        break;

    case kL3:
        posisi_servo[15] = theta_t_real;
        posisi_servo[16] = theta_f_real;
        posisi_servo[17] = theta_c_real;

        speed_servo[15] = kecepatan;
        speed_servo[16] = kecepatan;
        speed_servo[17] = kecepatan;

        L3_x = x;
        L3_y = y;
        L3_z = z;
        break;
    }

    // First send speed values
    for (int i = 0; i < 18; i++) {
        // Make sure we never use speed value 0 (which means max speed)
        if (speed_servo[i] == 0) {
            speed_servo[i] = 1; // Set to minimum speed instead of 0
        }
        
        param_goal_speed[0] = DXL_LOBYTE(speed_servo[i]);
        param_goal_speed[1] = DXL_HIBYTE(speed_servo[i]);
        groupSyncWriteSpeed.addParam(dxl_idku[i], param_goal_speed);
    }
    groupSyncWriteSpeed.txPacket();
    groupSyncWriteSpeed.clearParam();

    // Add a small delay to ensure speed command takes effect
    usleep(5000); // 5ms delay

    // Then send position values
    for (int i = 0; i < 18; i++) {
        param_goal_position[0] = DXL_LOBYTE(posisi_servo[i]);
        param_goal_position[1] = DXL_HIBYTE(posisi_servo[i]);
        groupSyncWrite.addParam(dxl_idku[i], param_goal_position);
    }
    groupSyncWrite.txPacket();
    groupSyncWrite.clearParam();

}


void polinomial_trj(uint32_t dxl_id, float xp1, float yp1, float zp1, float xp2, float yp2, float zp2, float xp3, float yp3, float zp3, float xp4, float yp4, float zp4)
{
    float A, B, C, D; // utk perhitungan polinomial
    float px, py, pz; // hasil polinomial
    int nmr_data = 0; // no data array

    // hitung end point dengan polinomial

    for (float t = 0.0; t <= 1.009; t = t + iterasi)
    {
        // hitung polinomial
        A = pow((1 - t), 3);
        B = 3 * t * pow(1 - t, 2);
        C = 3 * pow(t, 2) * (1 - t);
        D = pow(t, 3);
        px = A * xp1 + B * xp2 + C * xp3 + D * xp4;
        py = A * yp1 + B * yp2 + C * yp3 + D * yp4;
        pz = A * zp1 + B * zp2 + C * zp3 + D * zp4;

        // simpan hasil perhitungan
        switch (dxl_id)
        {

        case kR1:
            array_px_R1[nmr_data] = px;
            array_py_R1[nmr_data] = py;
            array_pz_R1[nmr_data] = pz;
            break;
        case kR2:
            array_px_R2[nmr_data] = px;
            array_py_R2[nmr_data] = py;
            array_pz_R2[nmr_data] = pz;
            break;
        case kR3:
            array_px_R3[nmr_data] = px;
            array_py_R3[nmr_data] = py;
            array_pz_R3[nmr_data] = pz;
            break;
        case kL1:
            array_px_L1[nmr_data] = px;
            array_py_L1[nmr_data] = py;
            array_pz_L1[nmr_data] = pz;
            break;
        case kL2:
            array_px_L2[nmr_data] = px;
            array_py_L2[nmr_data] = py;
            array_pz_L2[nmr_data] = pz;
            break;
        case kL3:
            array_px_L3[nmr_data] = px;
            array_py_L3[nmr_data] = py;
            array_pz_L3[nmr_data] = pz;
            break;
        }

        nmr_data++;
    }

    jlh_data = nmr_data;
}


void trj_lurus(uint32_t dxl_id, float x0, float y0, float z0, float x1, float y1, float z1)
{
    float xp1, yp1, zp1; // titik vektor1
    float xp2, yp2, zp2; // titik vektor2
    float xp3, yp3, zp3; // titik vektor3
    float xp4, yp4, zp4; // titik vektor4

    // tentukan titik vektor polinomial
    xp1 = x0;
    yp1 = y0;
    zp1 = z0; // P1
    xp2 = x0;
    yp2 = y0;
    zp2 = z0; // P2
    xp3 = x1;
    yp3 = y1;
    zp3 = z1; // P3
    xp4 = x1;
    yp4 = y1;
    zp4 = z1; // P4
    polinomial_trj(dxl_id, xp1, yp1, zp1, xp2, yp2, zp2, xp3, yp3, zp3, xp4, yp4, zp4);
}


// trayektori langkah
void trj_langkah(uint32_t dxl_id, float x0, float y0, float z0, float x1, float y1, float zp)
{
    float xp1, yp1, zp1; // titik vektor1
    float xp2, yp2, zp2; // titik vektor2
    float xp3, yp3, zp3; // titik vektor3
    float xp4, yp4, zp4; // titik vektor4

    float z1;
    z1 = (zp - (0.25 * z0)) / 0.75;
    // tentukan titik vektor polinomial
    xp1 = x0;
    yp1 = y0;
    zp1 = z0; // P1
    xp2 = x0;
    yp2 = y0;
    zp2 = z1; // P2
    xp3 = x1;
    yp3 = y1;
    zp3 = z1; // P3
    xp4 = x1;
    yp4 = y1;
    zp4 = z0; // P4
    polinomial_trj(dxl_id, xp1, yp1, zp1, xp2, yp2, zp2, xp3, yp3, zp3, xp4, yp4, zp4);
}


// eksekusi trayektori
void trj_start(uint32_t id_kakiR1, uint32_t id_kakiR2, uint32_t id_kakiR3, uint32_t id_kakiL1, uint32_t id_kakiL2, uint32_t id_kakiL3, int kecepatan, int base_delay_trj)
{
    // Calculate delay based on speed
    // For maximum speed (1023), use base_delay_trj
    // For slower speeds, increase delay proportionally
    int adjusted_delay = base_delay_trj * (1023.0 / kecepatan);

    // Limit to reasonable values
    if (adjusted_delay < base_delay_trj) adjusted_delay = base_delay_trj;
    if (adjusted_delay > base_delay_trj * 10) adjusted_delay = base_delay_trj * 10;

    // Hitung hasil perhitungan tsb menggunakan IK
    for (int i = 0; i < jlh_data; i++)
    {
        // Make sure kecepatan is in the correct range (1-1023, not 0)
        int adjusted_speed = kecepatan;
        if (adjusted_speed <= 0) adjusted_speed = 1; // Prevent using 0 which is maximum speed
        if (adjusted_speed > 1023) adjusted_speed = 1023; // Ensure we don't exceed the maximum

        if (id_kakiR1 == kR1)
        {
            inverse_k(id_kakiR1, array_px_R1[i], array_py_R1[i], array_pz_R1[i], kecepatan);
        }

        if (id_kakiR2 == kR2)
        {
            inverse_k(id_kakiR2, array_px_R2[i], array_py_R2[i], array_pz_R2[i], kecepatan);
        }

        if (id_kakiR3 == kR3)
        {
            inverse_k(id_kakiR3, array_px_R3[i], array_py_R3[i], array_pz_R3[i], kecepatan);
        }

        if (id_kakiL1 == kL1)
        {
            inverse_k(id_kakiL1, array_px_L1[i], array_py_L1[i], array_pz_L1[i], kecepatan);
        }

        if (id_kakiL2 == kL2)
        {
            inverse_k(id_kakiL2, array_px_L2[i], array_py_L2[i], array_pz_L2[i], kecepatan);
        }

        if (id_kakiL3 == kL3)
        {
            inverse_k(id_kakiL3, array_px_L3[i], array_py_L3[i], array_pz_L3[i], kecepatan);
        }

        usleep(adjusted_delay);  // Use adjusted delay
    }
}


void translasi_body(float pos_x, float pos_y, float pos_z , int kecepatan, int dly_trj) {

  //hitung posisi relatif thdp bodi
  kaki_to_body();

  //hitung koordinat selanjutnya
//kaki R1
  n_body_R1_x = body_R1_x + pos_x;
  n_body_R1_y = body_R1_y + pos_y;
  n_body_R1_z = body_R1_z + pos_z;
  //kaki R2
  n_body_R2_x = body_R2_x + pos_x;
  n_body_R2_y = body_R2_y + pos_y;
  n_body_R2_z = body_R2_z + pos_z;
  //kaki R3
  n_body_R3_x = body_R3_x + pos_x;
  n_body_R3_y = body_R3_y + pos_y;
  n_body_R3_z = body_R3_z + pos_z;
  //kaki L1
  n_body_L1_x = body_L1_x + pos_x;
  n_body_L1_y = body_L1_y + pos_y;
  n_body_L1_z = body_L1_z + pos_z;
  //kaki L2
  n_body_L2_x = body_L2_x + pos_x;
  n_body_L2_y = body_L2_y + pos_y;
  n_body_L2_z = body_L2_z + pos_z;
  //kaki L3
  n_body_L3_x = body_L3_x + pos_x;
  n_body_L3_y = body_L3_y + pos_y;
  n_body_L3_z = body_L3_z + pos_z;
  
  //kembalikan koordinat ke pusat coxa
  kaki_to_coxa();

  //gerakkan kaki (pos sekarang ke pos baru)
  
  trj_lurus(kR1, R1_x, R1_y, R1_z, n_R1_x, n_R1_y, n_R1_z);
  trj_lurus(kR2, R2_x, R2_y, R2_z, n_R2_x, n_R2_y, n_R2_z);
  trj_lurus(kR3, R3_x, R3_y, R3_z, n_R3_x, n_R3_y, n_R3_z);
  trj_lurus(kL1, L1_x, L1_y, L1_z, n_L1_x, n_L1_y, n_L1_z);
  trj_lurus(kL2, L2_x, L2_y, L2_z, n_L2_x, n_L2_y, n_L2_z);
  trj_lurus(kL3, L3_x, L3_y, L3_z, n_L3_x, n_L3_y, n_L3_z);
  trj_start(kR1, kR2, kR3, kL1, kL2, kL3, kecepatan, dly_trj);

}


//hitung koordinat kaki terhadap pusat body
void kaki_to_body() {
  //kaki R1
  body_R1_x = R1_x + offset_R1_x;
  body_R1_y = R1_y + offset_R1_y;
  body_R1_z = R1_z + offset_R1_z;
  //kaki R2
  body_R2_x = R2_x + offset_R2_x;
  body_R2_y = R2_y + offset_R2_y;
  body_R2_z = R2_z + offset_R2_z;
  //kaki R3
  body_R3_x = R3_x + offset_R3_x;
  body_R3_y = R3_y + offset_R3_y;
  body_R3_z = R3_z + offset_R3_z;
  //kaki L1
  body_L1_x = L1_x + offset_L1_x;
  body_L1_y = L1_y + offset_L1_y;
  body_L1_z = L1_z + offset_L1_z;
  //kaki L2
  body_L2_x = L2_x + offset_L2_x;
  body_L2_y = L2_y + offset_L2_y;
  body_L2_z = L2_z + offset_L2_z;
  //kaki L3
  body_L3_x = L3_x + offset_L3_x;
  body_L3_y = L3_y + offset_L3_y;
  body_L3_z = L3_z + offset_L3_z;
  
}


//hitung koordinat kaki terhadap pusat coxa (Inverse kinematics)
void kaki_to_coxa() {
  //koordinat kaki baru
  //kaki R1
  n_R1_x = n_body_R1_x - offset_R1_x;
  n_R1_y = n_body_R1_y - offset_R1_y;
  n_R1_z = n_body_R1_z - offset_R1_z;
  //kaki R2
  n_R2_x = n_body_R2_x - offset_R2_x;
  n_R2_y = n_body_R2_y - offset_R2_y;
  n_R2_z = n_body_R2_z - offset_R2_z;
  //kaki R3
  n_R3_x = n_body_R3_x - offset_R3_x;
  n_R3_y = n_body_R3_y - offset_R3_y;
  n_R3_z = n_body_R3_z - offset_R3_z;
  //kaki L1
  n_L1_x = n_body_L1_x - offset_L1_x;
  n_L1_y = n_body_L1_y - offset_L1_y;
  n_L1_z = n_body_L1_z - offset_L1_z;
  //kaki L2
  n_L2_x = n_body_L2_x - offset_L2_x;
  n_L2_y = n_body_L2_y - offset_L2_y;
  n_L2_z = n_body_L2_z - offset_L2_z;
  //kaki L3
  n_L3_x = n_body_L3_x - offset_L3_x;
  n_L3_y = n_body_L3_y - offset_L3_y;
  n_L3_z = n_body_L3_z - offset_L3_z;
  
}


void siap()
{
    trj_lurus(kR1, 80, 0, -60, 80, 0, -60);
    trj_lurus(kR2, 80, 0, -60, 80, 0, -60);
    trj_lurus(kR3, 80, 0, -60, 80, 0, -60);

    trj_lurus(kL1, 95, 0, -60, 95, 0, -60);
    trj_lurus(kL2, 80, 0, -60, 80, 0, -60);
    trj_lurus(kL3, 80, 0, -60, 80, 0, -60);

    trj_start(kR1, kR2, kR3, kL1, kL2, kL3, ultra_slow, 2500);
}


void coba_majuv2()
{
    // kanan
    trj_langkah(kL1, L1_x, L1_y, L1_z, 80, 25, -40);
    trj_langkah(kL3, L3_x, L3_y, L3_z, 80, -25, -40);
    trj_langkah(kR2, R2_x, R2_y, R2_z, 80, 15, -40);

    trj_lurus(kR1, R1_x, R1_y, R1_z, 80, -40, -60); // 90
    trj_lurus(kR3, R3_x, R3_y, R3_z, 80, 10, -60);  // 90
    trj_lurus(kL2, L2_x, L2_y, L2_z, 80, 0, -60);
    trj_start(kR1, kR2, kR3, kL1, kL2, kL3, ultra_fast, 2500);

    trj_langkah(kR1, R1_x, R1_y, R1_z, 80, -25, -40);
    trj_langkah(kR3, R3_x, R3_y, R3_z, 80, 25, -40);
    trj_langkah(kL2, L2_x, L2_y, L2_z, 80, -15, -40);

    trj_lurus(kL1, L1_x, L1_y, L1_z, 80, 40, -60);  // 95
    trj_lurus(kL3, L3_x, L3_y, L3_z, 80, -10, -60); // 100
    trj_lurus(kR2, R2_x, R2_y, R2_z, 80, 0, -60);
    trj_start(kR1, kR2, kR3, kL1, kL2, kL3, ultra_fast, 2500);
}


void lift_front_legs() {
    // Get current positions to preserve them
    //front legs
    float R1_x_current = R1_x;
    float R1_y_current = R1_y;
    float R1_z_current = R1_z;

    float L1_x_current = L1_x;
    float L1_y_current = L1_y;
    float L1_z_current = L1_z;

    //back legs
    float R3_x_current = R3_x;
    float R3_y_current = R3_y;
    float R3_z_current = R3_z;

    float L3_x_current = L3_x;
    float L3_y_current = L3_y;
    float L3_z_current = L3_z;

    // Lift the legs by directly calling inverse_k
    // This will move just the specified legs to a higher position
    // The -30 value will lift the legs higher than the default -60 (the smaller, the higher [I guess... idk let's just skibidi it all the way through])
    inverse_k(kR1, R1_x_current, R1_y_current, -35, very_slow);
    inverse_k(kL1, L1_x_current, L1_y_current, -35, very_slow);

    inverse_k(kR3, R3_x_current, R3_y_current, -85, very_slow);
    inverse_k(kL3, L3_x_current, L3_y_current, -85, very_slow);
    
    // Wait while legs are up
    usleep(2000000); // 2 seconds
    
    // Return legs to original position
    inverse_k(kR1, R1_x_current, R1_y_current, R1_z_current, very_slow);
    inverse_k(kL1, L1_x_current, L1_y_current, L1_z_current, very_slow);

    inverse_k(kR3, R3_x_current, R3_y_current, R3_z_current, very_slow);
    inverse_k(kL3, L3_x_current, L3_y_current, L3_z_current, very_slow);
}


void lift_middle_legs() {
    // Get current positions to preserve them
    float R2_x_current = R2_x;
    float R2_y_current = R2_y;
    float R2_z_current = R2_z;

    float L2_x_current = L2_x;
    float L2_y_current = L2_y;
    float L2_z_current = L2_z;

    // Lift the legs by directly calling inverse_k
    // This will move just the specified legs to a higher position
    // The -30 value will lift the legs higher than the default -60 (the smaller, the higher [I guess... idk let's just skibidi it all the way through])
    inverse_k(kR2, R2_x_current, R2_y_current, -40, very_slow);
    inverse_k(kL2, L2_x_current, L2_y_current, -40, very_slow);
    
    // Wait while legs are up
    usleep(2000000); // 2 seconds
    
    // Return legs to original position
    inverse_k(kR2, R2_x_current, R2_y_current, R2_z_current, very_slow);
    inverse_k(kL2, L2_x_current, L2_y_current, L2_z_current, very_slow);
}


void lift_back_legs() {
    // Get current positions to preserve them
    //front legs
    float R1_x_current = R1_x;
    float R1_y_current = R1_y;
    float R1_z_current = R1_z;

    float L1_x_current = L1_x;
    float L1_y_current = L1_y;
    float L1_z_current = L1_z;

    //back legs
    float R3_x_current = R3_x;
    float R3_y_current = R3_y;
    float R3_z_current = R3_z;

    float L3_x_current = L3_x;
    float L3_y_current = L3_y;
    float L3_z_current = L3_z;

    // Lift the legs by directly calling inverse_k
    // This will move just the specified legs to a higher position
    // The -30 value will lift the legs higher than the default -60 (the smaller, the higher [I guess... idk let's just skibidi it all the way through])
    inverse_k(kR1, R1_x_current, R1_y_current, -85, very_slow);
    inverse_k(kL1, L1_x_current, L1_y_current, -85, very_slow);

    inverse_k(kR3, R3_x_current, R3_y_current, -35, very_slow);
    inverse_k(kL3, L3_x_current, L3_y_current, -35, very_slow);
    
    // Wait while legs are up
    usleep(2000000); // 2 seconds
    
    // Return legs to original position
    inverse_k(kR1, R1_x_current, R1_y_current, R1_z_current, very_slow);
    inverse_k(kL1, L1_x_current, L1_y_current, L1_z_current, very_slow);

    inverse_k(kR3, R3_x_current, R3_y_current, R3_z_current, very_slow);
    inverse_k(kL3, L3_x_current, L3_y_current, L3_z_current, very_slow);
}

void miring_kiri() {
    //priming
    siap();
    // ingat posisi kaki sebelum mulai
    //front
    float R1_x_current = R1_x;
    float R1_y_current = R1_y;
    float R1_z_current = R1_z;

    float L1_x_current = L1_x;
    float L1_y_current = L1_y;
    float L1_z_current = L1_z;

    //mid
    float R2_x_current = R2_x;
    float R2_y_current = R2_y;
    float R2_z_current = R2_z;

    float L2_x_current = L2_x;
    float L2_y_current = L2_y;
    float L2_z_current = L2_z;

    //rear
    float R3_x_current = R3_x;
    float R3_y_current = R3_y;
    float R3_z_current = R3_z;

    float L3_x_current = L3_x;
    float L3_y_current = L3_y;
    float L3_z_current = L3_z;

    //delay sebentar (make sure are legs are ready)
    usleep(500000);

    //begin translation
    inverse_k(kR1, R1_x_current, R1_y_current, R1_z_current, very_slow);
    inverse_k(kL1, L1_x_current, L1_y_current, -30, very_slow);

    inverse_k(kR2, R2_x_current, R2_y_current, R2_z_current, very_slow);
    inverse_k(kL2, L2_x_current, L2_y_current, -30, very_slow);

    inverse_k(kR3, R3_x_current, R3_y_current, R3_z_current, very_slow);
    inverse_k(kL3, L3_x_current, L3_y_current, -30, very_slow);

    //delay before returning back to normal
    usleep(1500000);

    //back to posisi sebelumnya
    inverse_k(kR1, R1_x_current, R1_y_current, R1_z_current, very_slow);
    inverse_k(kL1, L1_x_current, L1_y_current, L1_z_current, very_slow);

    inverse_k(kR2, R2_x_current, R2_y_current, R2_z_current, very_slow);
    inverse_k(kL2, L2_x_current, L2_y_current, L2_z_current, very_slow);

    inverse_k(kR3, R3_x_current, R3_y_current, R3_z_current, very_slow);
    inverse_k(kL3, L3_x_current, L3_y_current, L3_z_current, very_slow);
}

void miring_kanan() {
    // ingat posisi kaki sebelum mulai
    //front
    float R1_x_current = R1_x;
    float R1_y_current = R1_y;
    float R1_z_current = R1_z;

    float L1_x_current = L1_x;
    float L1_y_current = L1_y;
    float L1_z_current = L1_z;

    //mid
    float R2_x_current = R2_x;
    float R2_y_current = R2_y;
    float R2_z_current = R2_z;

    float L2_x_current = L2_x;
    float L2_y_current = L2_y;
    float L2_z_current = L2_z;

    //rear
    float R3_x_current = R3_x;
    float R3_y_current = R3_y;
    float R3_z_current = R3_z;

    float L3_x_current = L3_x;
    float L3_y_current = L3_y;
    float L3_z_current = L3_z;

    //delay bentar (u know why)
    usleep(500000);

    //begin translation
    inverse_k(kR1, R1_x_current, R1_y_current, -30, very_slow);
    inverse_k(kL1, L1_x_current, L1_y_current, L1_z_current, very_slow);

    inverse_k(kR2, R2_x_current, R2_y_current, -30, very_slow);
    inverse_k(kL2, L2_x_current, L2_y_current, L2_z_current, very_slow);

    inverse_k(kR3, R3_x_current, R3_y_current, -30, very_slow);
    inverse_k(kL3, L3_x_current, L3_y_current, L3_z_current, very_slow);

    //delay before returning back to normal
    usleep(1500000);

    //back to posisi sebelumnya
    inverse_k(kR1, R1_x_current, R1_y_current, R1_z_current, very_slow);
    inverse_k(kL1, L1_x_current, L1_y_current, L1_z_current, very_slow);

    inverse_k(kR2, R2_x_current, R2_y_current, R2_z_current, very_slow);
    inverse_k(kL2, L2_x_current, L2_y_current, L2_z_current, very_slow);

    inverse_k(kR3, R3_x_current, R3_y_current, R3_z_current, very_slow);
    inverse_k(kL3, L3_x_current, L3_y_current, L3_z_current, very_slow);
}

void translation_test() {
    // make sure robot is primed
    siap();

    // remember last leg position
    float R1_x_current = R1_x;
    float R1_y_current = R1_y;
    float R1_z_current = R1_z;

    float L1_x_current = L1_x;
    float L1_y_current = L1_y;
    float L1_z_current = L1_z;

    float R2_x_current = R2_x;
    float R2_y_current = R2_y;
    float R2_z_current = R2_z;

    float L2_x_current = L2_x;
    float L2_y_current = L2_y;
    float L2_z_current = L2_z;

    float R3_x_current = R3_x;
    float R3_y_current = R3_y;
    float R3_z_current = R3_z;

    float L3_x_current = L3_x;
    float L3_y_current = L3_y;
    float L3_z_current = L3_z;

    //delay bentar
    usleep(500000);

    //begin translation
    inverse_k(kR1, R1_x_current, -50, R1_z_current, very_slow);
    inverse_k(kL1, L1_x_current, 50, L1_z_current, very_slow);

    inverse_k(kR2, R2_x_current, -45, R2_z_current, very_slow);
    inverse_k(kL2, L2_x_current, 45, L2_z_current, very_slow);

    inverse_k(kR3, R3_x_current, -25, R3_z_current, very_slow);
    inverse_k(kL3, L3_x_current, 25, L3_z_current, very_slow);

    //delay translasi (durasinya)
    usleep(3500000);

    //back to posisi sebelumnya
    inverse_k(kR1, R1_x_current, R1_y_current, R1_z_current, very_slow);
    inverse_k(kL1, L1_x_current, L1_y_current, L1_z_current, very_slow);

    inverse_k(kR2, R2_x_current, R2_y_current, R2_z_current, very_slow);
    inverse_k(kL2, L2_x_current, L2_y_current, L2_z_current, very_slow);

    inverse_k(kR3, R3_x_current, R3_y_current, R3_z_current, very_slow);
    inverse_k(kL3, L3_x_current, L3_y_current, L3_z_current, very_slow);
}

void test1()
{
    inverse_k(kR1, 80, 0, -60, very_slow);
}
