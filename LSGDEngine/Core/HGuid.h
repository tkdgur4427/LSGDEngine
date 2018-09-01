#pragma once

namespace lsgd {

	class HGuid
	{
	public:
		// create guid with valid guid instance
		static HGuid CreateGuid();

		// note that when default constructor is called, Value == nullptr
		HGuid() {}
		HGuid(const HGuid& other);

		// guid real object
		struct GuidInstance {};

		// methods
		HString ToString() const;
		operator HString() const;

		const HFixedArray<uint8, 16>& ToArray() const;
		void Swap(HGuid& other);
		bool IsValid() const;

		// operators
		HGuid& operator=(const HGuid& other);
		bool operator==(const HGuid& other) const;
		bool operator!=(const HGuid& other) const;

	protected:		
		shared_ptr<GuidInstance> Value;

		explicit HGuid(shared_ptr<GuidInstance>& InValue)
			: Value(InValue)
		{}
	};
}

namespace std
{
	template <>
	void swap(lsgd::HGuid &guid0, lsgd::HGuid &guid1);

	// Specialization for std::hash<Guid> -- this implementation
	// uses std::hash<std::string> on the stringification of the guid
	// to calculate the hash
	template <>
	struct hash<lsgd::HGuid>
	{
		typedef lsgd::HGuid argument_type;
		typedef std::size_t result_type;

		result_type operator()(argument_type const &guid) const
		{
			std::hash<std::string> hasher;
			return static_cast<result_type>(hasher(guid.ToString()));
		}
	};
}