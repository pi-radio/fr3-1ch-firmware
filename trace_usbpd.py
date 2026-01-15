#!/usr/bin/env python3

import serial
import click


@click.command()
@click.argument("serial-port", type=click.Path(exists=True))
def trace_usbpd(serial_port):
    ser = serial.Serial(serial_port, 921600)

    TLV_SOF=0xFD
    TLV_EOF=0xA5

    def get_ch():
        ch = ser.read(1)[0]
        return ch

    trace_types = {
        0: "FORMAT_TLV",
        1: "MSG IN",
        2: "MSG OUT",
        3: "CAD EVENT",
        4: "PE STATE",
        5: "CAD LOW",
        6: "DEBUG",
        7: "SRC",
        8: "SNK",
        9: "NOTIF",
        10: "POWER",
        11: "TCPM",
        12: "PRL STATE",
        13: "PRL EVENT",
        14: "PHY NOTFRWD",
        15: "CPU",
        16: "TIMEOUT",
        18: "UCSI"
    }

    while True:
        sof_count = 0

        while sof_count < 4:
            sof = get_ch()
            
            if sof_count == 0 and sof == 0xFF:
                # Not sure how they get here
                pass
            elif sof != TLV_SOF:
                print(f"Invalid SOF: {sof}")
                sof_count = 0
            else:
                sof_count += 1

        tag = get_ch()

        len_ch = ser.read(2)

        tuple_len = int.from_bytes(len_ch, 'big')

        print(f"Tag: {tag} Length: {tuple_len}")

        data = ser.read(tuple_len)

        trace_type = data[0]

        time = int.from_bytes(data[1:5], "little")

        port = data[5]

        sop = data[6]

        size2 = int.from_bytes(data[7:9], "big")

        print(f"Trace: type: {trace_types[trace_type]} time: {time} port: {port} sop: {sop} size2: {size2}")

        if size2 > 0:
            try:
                s = data[9:9+size2].decode('utf-8')

                if s[-1] == '\n':
                    s = s[:-1]
        
                print(s)
            except:
                print(f"Failed to decode: {s}")
    
        for _ in range(4):
            eof = get_ch()

            if eof != TLV_EOF:
                print(f"Invalid EOF: {eof}")

        
    
trace_usbpd()
