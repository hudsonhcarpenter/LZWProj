<h1>Lempel-Ziv-Welch Compression Implementation</h1>

<h2>Description</h2>
This project implements the Lempel-Ziv-Welch (LZW) data compression algorithm in C, featuring both an encoder and decoder for compressing and decompressing files. The project was developed in three stages, progressively improving the implementation by adding features like dynamic packing and table pruning.

<br />

<h2>Key Features</h2>

<ul>
        <li>✅ <strong>LZW Compression & Decompression</strong> – Implements LZW for lossless data compression, processing input from standard input and output.</li>
        <li>✅ <strong>Dynamic Bit Allocation</strong> – Adapts the number of bits per code dynamically, optimizing space usage.</li>
        <li>✅ <strong>Pruning Optimization (-p)</strong> – Removes unused entries from the dictionary to enhance efficiency.</li>
        <li>✅ <strong>Configurable Maximum Bit Width (-m MAXBITS)</strong> – Allows custom table size for different compression levels.</li>
        <li>✅ <strong>ASCII and Binary Support</strong> – Works with both text and binary files, making it versatile for different use cases.</li>
        <li>✅ <strong>Command-Line Interface (CLI)</strong> – Encodes and decodes data using intuitive CLI commands:
            <pre><code>encode [-m MAXBITS] [-p]
decode</code></pre>
        </li>
        <li>✅ <strong>Efficient Data Structures</strong> – Uses hashing with chaining for fast dictionary lookups during encoding.</li>
        <li>✅ <strong>Batch Mode Processing</strong> – Supports reading and writing compressed data efficiently for automated execution.</li>
    </ul>
