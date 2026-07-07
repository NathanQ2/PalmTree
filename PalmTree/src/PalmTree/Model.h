#pragma once

namespace PalmTree {
    class VertexBuffer;
    class IndexBuffer;
    struct Vertex;

    class Model {
    public:
        struct Builder {
            std::vector<Vertex> Vertices{};
            std::vector<uint32_t> Indices{};

            void LoadModel(const std::string& path);
        };

        Model(const Builder& builder);
        ~Model();

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        static std::unique_ptr<Model> CreateModelFromFile(const std::string& path);

        bool HasIndexBuffer() const { return m_HasIndexBuffer; }
        const VertexBuffer& GetVertexBuffer() const { return *m_VertexBuffer; }
        const IndexBuffer& GetIndexBuffer() const { return *m_IndexBuffer; }

        uint32_t GetVertexCount() const;
        uint32_t GetIndexCount() const;
    private:
        void CreateVertexBuffers(const std::vector<Vertex>& vertices);
        void CreateIndexBuffers(const std::vector<uint32_t>& indices);

        std::unique_ptr<VertexBuffer> m_VertexBuffer;

        bool m_HasIndexBuffer = false;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;
    };
}
