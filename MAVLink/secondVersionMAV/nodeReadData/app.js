const http = require('http')
const express = require('express')
const socketIO = require('socket.io')

const app = express()
const server = http.createServer(app)
const io = socketIO.listen(server)

app.use(express.static(__dirname + '/public'))

app.get('/', (req, res) => {
    res.sendfile('index.html')
})

// Serial
    const Serialport = require('serialport')
    const port = new Serialport('COM15', {
        baudRate: 9600
    })

    const readLine = Serialport.parsers.Readline
    const parser = port.pipe(new readLine({ delimiter: '\n'}))

    parser.on('open', () => {
        console.log('Open port!')
    })

    parser.on('data', (data) => {
        repackedData = parserData(data)
        //console.log(repackedData)
        //console.log(data)
        io.emit('dataArduino', repackedData)
    })

    const parserData = (data) => {
        splited = data.split(';')

        channels = splited[0]
        channels = channels.split(',')

        vrf = splited[1]
        vrf = vrf.split(',')

        attitude = splited[2]
        attitude = attitude.split(',')

        gps = splited[3]
        gps = gps.split(',')

        sys = splited[4]
        sys = sys.split(',')

        repackedData = {
            channels,
            vrf,
            attitude,
            gps,
            sys,
        }
        return repackedData
    }

// socket
    //io.on('connection', (socket) => {
    //    console.log('connected')
    //})

server.listen(3000, () => {
    console.log('Server listen!')
})