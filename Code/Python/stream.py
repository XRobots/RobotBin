import pyaudio
import asyncio
import sys
import websockets
import time
import os
import json
import serial

#setup serial
arduino = serial.Serial(port='/dev/ttyUSB1', baudrate=115200, timeout=.1)

FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 44100
CHUNK = 8192

async def run():
    audio = pyaudio.PyAudio()

    audio_cursor = 0.

    extra_headers = {
        'Authorization': 'token ' + os.environ.get('DG_API_KEY')
    }
    async with websockets.connect('wss://api.deepgram.com/v1/listen?encoding=linear16&sample_rate=44100&channels=1&language=en-GB', extra_headers = extra_headers) as ws:
        stream = audio.open(
            format = FORMAT,
            channels = CHANNELS,
            rate = RATE,
            input = True,
            frames_per_buffer = CHUNK
        )

        async def sender(ws):
            nonlocal audio_cursor
            try:
                nonlocal stream
                while True:
                    data = stream.read(CHUNK)
                    await ws.send(data)
                    audio_cursor += float(CHUNK) / float(RATE)
                    await asyncio.sleep(float(CHUNK) / float(RATE))
            except Exception as e:
                print('Error while sending: ', + string(e))
                raise

        async def receiver(ws):
            nonlocal audio_cursor
            transcript_cursor = 0.
            async for msg in ws:
                msg = json.loads(msg)

                transcript_cursor = msg['start'] + msg['duration']

                cursor_latency = audio_cursor - transcript_cursor

                transcript = msg['channel']['alternatives'][0]['transcript']

                print(f'Transcript = {transcript}; Measuring... Audio cursor = {audio_cursor:.3f}, Transcript cursor = {transcript_cursor:.3f}, Cursor latency = {cursor_latency:.3f}')        
                print(transcript)
                chara = str(-20)
                if transcript == 'go home':
                    char1 = str(1)
                    print(1)
                elif transcript == 'location one':
                    char1 = str(2)
                    print(2)
                elif transcript == 'come to the sink':
                    char1 = str(3)
                    print(3)
                elif transcript == 'location two':
                    char1 = str(4)
                    print(4)
                else:
                    char1 = str(0)
                arduino.write(bytes(chara, 'utf8'))
                arduino.write(bytes(',', 'utf8'))
                arduino.write(bytes(char1, 'utf8'))
                arduino.write(bytes(',', 'utf8'))
 

        await asyncio.wait([
            asyncio.ensure_future(sender(ws)),
            asyncio.ensure_future(receiver(ws))
        ])

def main():
    loop = asyncio.get_event_loop()
    asyncio.get_event_loop().run_until_complete(run())

if __name__ == '__main__':
    sys.exit(main() or 0)

